// Copyright Pico Technology Co, Ltd. All Rights Reserved.

#include "PicoMobile.h"

#include "PicoImport.h"
#include "PicoMobileSettings.h"
#include "PicoJava.h"
#include "PicoEyeTracker.h"

#include "Modules/ModuleInterface.h"
#include "Modules/ModuleManager.h"

#if WITH_EDITOR
#include "ISettingsModule.h"
#include "ISettingsSection.h"
#endif

#include "PostProcess/PostProcessHMD.h"
#include "RendererPrivate.h"

#include "../Launch/Resources/Version.h"


DEFINE_LOG_CATEGORY_STATIC(LogPicoHMD, Log, All);

#define LOCTEXT_NAMESPACE "FPicoMobilePlugin"

///////////////////////
// FPicoMobilePlugin //
///////////////////////

class FPicoMobilePlugin : public IPicoMobilePlugin
{
	virtual TSharedPtr< class IXRTrackingSystem, ESPMode::ThreadSafe > CreateTrackingSystem() override;

	FString GetModuleKeyName() const override
	{
		return FString(TEXT("PicoMobile"));
	}

	virtual void StartupModule() override
	{
		IHeadMountedDisplayModule::StartupModule();
#if WITH_EDITOR
		// register settings
		ISettingsModule* SettingsModule = FModuleManager::GetModulePtr<ISettingsModule>(FName("Settings"));

		if (SettingsModule != nullptr)
		{
			ISettingsSectionPtr SettingsSection = SettingsModule->RegisterSettings("Project", "Plugins", "PicoMobile",
				LOCTEXT("PicoMobileSettingsName", "PicoMobile"),
				LOCTEXT("PicoMobileSettingsDescription", "Configure the PicoMobile plug-in."),
				GetMutableDefault<UPicoMobileSettings>()
			);
		}
#endif // WITH_EDITOR

	}

	virtual void ShutdownModule() override
	{
		IHeadMountedDisplayModule::ShutdownModule();
#if WITH_EDITOR
		// unregister settings
		ISettingsModule* SettingsModule = FModuleManager::GetModulePtr<ISettingsModule>("Settings");

		if (SettingsModule != nullptr)
		{
			SettingsModule->UnregisterSettings("Project", "Plugins", "PicoMobile");
		}
#endif

	}

	virtual bool SupportsDynamicReloading() override
	{
		return false;
	}

};

IMPLEMENT_MODULE(FPicoMobilePlugin, PicoMobile)

TSharedPtr< class IXRTrackingSystem, ESPMode::ThreadSafe > FPicoMobilePlugin::CreateTrackingSystem()
{
	TSharedPtr< FPicoMobileHMD, ESPMode::ThreadSafe > PicoMobileHMD = FSceneViewExtensions::NewExtension<FPicoMobileHMD>();
	if (PicoMobileHMD->IsInitialized())
	{
		return PicoMobileHMD;
	}
	return nullptr;
}


////////////////////
// FPicoMobileHMD //
////////////////////

FPicoMobileHMD::FPicoMobileHMD(const FAutoRegister& AutoRegister) :
#if ENGINE_MINOR_VERSION >= 22
	FHeadMountedDisplayBase(nullptr),
#endif
	FSceneViewExtensionBase(AutoRegister),
	isStereoEnabled(true),
	CurHmdOrientation(FQuat::Identity),
	LastHmdOrientation(FQuat::Identity),
	DeltaControlRotation(FRotator::ZeroRotator),
	DeltaControlOrientation(FQuat::Identity),
	LastSensorTime(-1.0),
	bUsesArrayTexture(false),
	FixedFrameRate(62.0),
	UseFixedFrameRate(false),
	EyeTracker(nullptr),
	PixelDensity(1.0f),
	NextPicoLayerId(0),
	isFixOverlay(false),
	InitCamPos(FVector::ZeroVector),
	PreCamPos(FVector::ZeroVector)
{
	EyeTracker = FPicoEyeTracker::GetInstance();
	Startup();

#if PLATFORM_ANDROID
	// Safe Panel
	if (!GetDefault <UPicoMobileSettings>()->disableHmd6DofModule)
	{
		FPicoJava::IsHead6dofReset();
	}

	// SDK Version
	FString sdkVersion = FString(PicoImport::PvrGetSDKVersion());
	UE_LOG(LogPicoHMD, Log, TEXT("SDK Version: %s; Plugin Version: 1.2.0.2"), *sdkVersion);
#endif
}

FPicoMobileHMD::~FPicoMobileHMD()
{
	EyeTracker->Destory();
	Shutdown();
}

// IXRSystemIdentifier

FName FPicoMobileHMD::GetSystemName() const
{
	static FName DefaultName(TEXT("FPicoVRHMD"));
	return DefaultName;
}

// IXRTrackingSystem

bool FPicoMobileHMD::DoesSupportPositionalTracking() const
{
	return !(GetDefault <UPicoMobileSettings>()->disableHmd6DofModule);
}

bool FPicoMobileHMD::EnumerateTrackedDevices(TArray<int32>& OutDevices, EXRTrackedDeviceType Type)
{
	if (Type == EXRTrackedDeviceType::Any || Type == EXRTrackedDeviceType::HeadMountedDisplay)
	{
		OutDevices.Add(IXRTrackingSystem::HMDDeviceId);
		return true;
	}
	return false;
}


bool FPicoMobileHMD::GetCurrentPose(int32 DeviceId, FQuat & OutOrientation, FVector & OutPosition)
{
	if (DeviceId != HMDDeviceId)
	{
		return false;
	}

#if PLATFORM_ANDROID
	if (!PicoSDK::GIsSDKInit && EyeTracker->ActivePlayerController != NULL)
	{
			OutPosition = EyeTracker->ActivePlayerController->PlayerCameraManager->GetCameraLocation();
			OutOrientation = EyeTracker->ActivePlayerController->PlayerCameraManager->GetCameraRotation().Quaternion();
	}
	else
	{
		OutPosition = PicoSDK::Sensor::GCurrentPosition;
		OutOrientation = PicoSDK::Sensor::GCurrentOrientation;
	}
	CurHmdOrientation = LastHmdOrientation = PicoSDK::Sensor::GCurrentOrientation;
	return true;
#else
	ULocalPlayer* Player = GEngine->GetDebugLocalPlayer();

	if (Player != NULL && Player->PlayerController != NULL)
	{
		FVector RotationRate = Player->PlayerController->GetInputVectorKeyState(EKeys::RotationRate);

		double CurrentTime = FApp::GetCurrentTime();
		double DeltaTime = 0.0;

		if (LastSensorTime >= 0.0)
		{
			DeltaTime = CurrentTime - LastSensorTime;
		}

		LastSensorTime = CurrentTime;

		// mostly incorrect, but we just want some sensor input for testing
		RotationRate *= DeltaTime;
		CurHmdOrientation *= FQuat(FRotator(FMath::RadiansToDegrees(-RotationRate.X), FMath::RadiansToDegrees(-RotationRate.Y), FMath::RadiansToDegrees(-RotationRate.Z)));
}
	else
	{
		CurHmdOrientation = FQuat(FRotator(0.0f, 0.0f, 0.0f));
	}
	return true;
#endif
}

void FPicoMobileHMD::SetTrackingOrigin(EHMDTrackingOrigin::Type NewOrigin)
{
#if PLATFORM_ANDROID
	if (NewOrigin == EHMDTrackingOrigin::Eye) {
		PicoImport::PvrSetTrackingOrigin(0);
		PicoSDK::Sensor::TrackingOrigin = 0;
	}
	else
	{
		PicoImport::PvrSetTrackingOrigin(1);
		PicoSDK::Sensor::TrackingOrigin = 1;
	}
#endif
}

#if ENGINE_MINOR_VERSION > 22
EHMDTrackingOrigin::Type FPicoMobileHMD::GetTrackingOrigin() const
#else
EHMDTrackingOrigin::Type FPicoMobileHMD::GetTrackingOrigin()
#endif
{
#if PLATFORM_ANDROID
	if (PicoSDK::Sensor::TrackingOrigin == 0) {
		return EHMDTrackingOrigin::Eye;
	}
	else {
		return EHMDTrackingOrigin::Floor;
	}
#endif
	return EHMDTrackingOrigin::Eye;
}

bool FPicoMobileHMD::GetRelativeEyePose(int32 DeviceId, EStereoscopicPass Eye, FQuat & OutOrientation, FVector & OutPosition)
{
	OutOrientation = FQuat::Identity;
	OutPosition = FVector::ZeroVector;
#if PLATFORM_ANDROID
	if (DeviceId == IXRTrackingSystem::HMDDeviceId && (Eye == eSSP_LEFT_EYE || Eye == eSSP_RIGHT_EYE))
	{
		OutPosition = FVector(0, (Eye == EStereoscopicPass::eSSP_LEFT_EYE ? -.5 : .5) * GetInterpupillaryDistance() * GetWorldToMetersScale(), 0);
		return true;
	}
	else
#endif
	{
		return false;
	}
}

void FPicoMobileHMD::ResetOrientationAndPosition(float yaw)
{
#if PLATFORM_ANDROID
	PicoSDK::Sensor::OptionalResetSensor(PicoSDK::Sensor::GMainSensorIndex, 1, 1);
#endif
}

void FPicoMobileHMD::ResetOrientation(float Yaw)
{
#if PLATFORM_ANDROID
	PicoSDK::Sensor::OptionalResetSensor(PicoSDK::Sensor::GMainSensorIndex, 1, 0);
#endif
}

void FPicoMobileHMD::ResetPosition()
{
#if PLATFORM_ANDROID
	PicoSDK::Sensor::OptionalResetSensor(PicoSDK::Sensor::GMainSensorIndex, 0, 1);
#endif
}

void FPicoMobileHMD::SetBaseRotation(const FRotator & BaseRot)
{
	SetBaseOrientation(FRotator(0.0f, BaseRot.Yaw, 0.0f).Quaternion());
}

FRotator FPicoMobileHMD::GetBaseRotation() const
{
	return GetBaseOrientation().Rotator();
}

void FPicoMobileHMD::SetBaseOrientation(const FQuat & BaseOrient)
{
	BaseOrientation = BaseOrient;
}

FQuat FPicoMobileHMD::GetBaseOrientation() const
{
	return BaseOrientation;
}

IHeadMountedDisplay * FPicoMobileHMD::GetHMDDevice()
{
	return this;
}

TSharedPtr<class IStereoRendering, ESPMode::ThreadSafe> FPicoMobileHMD::GetStereoRenderingDevice()
{
	return SharedThis(this);
}

bool FPicoMobileHMD::OnStartGameFrame(FWorldContext& WorldContext)
{
	EyeTracker->ActivePlayerController = GEngine->GetFirstLocalPlayerController(WorldContext.World());

#if PLATFORM_ANDROID
	if (PicoSDK::GIsMapPost)
	{
		if (PicoSDK::GMapInitFrame == 5)
		{
			if (MapToInitPos.Num() > 0 && MapToInitPos.Contains(WorldContext.World()->GetMapName()))
			{
				InitCamPos = *MapToInitPos.Find(WorldContext.World()->GetMapName());
			}
			else
			{
				InitCamPos = GEngine->GetFirstLocalPlayerController(WorldContext.World())->PlayerCameraManager->GetCameraLocation();
				InitCamPos = InitCamPos - PreCamPos;
				MapToInitPos.Add(WorldContext.World()->GetMapName(), InitCamPos);
			}
			PicoSDK::GIsMapPost = false;
			PicoSDK::GMapInitFrame = 1;
		}
		else
		{
			PreCamPos = PicoSDK::Sensor::GCurrentPosition;
			PicoSDK::GMapInitFrame++;
		}
	}

	if (!isStereoEnabled)
	{
		EnableStereo(true);
	}
#endif
	return true;
}

void FPicoMobileHMD::OnBeginRendering_RenderThread(FRHICommandListImmediate& RHICmdList, FSceneViewFamily& ViewFamily)
{
#if PLATFORM_ANDROID
	if (!PicoSDK::GIsSDKInit)
	{	
		//InitCamPos = GEngine->GetFirstLocalPlayerController(GEngine->GameViewport->GetWorld())->PlayerCameraManager->GetCameraLocation();

		SubmitUnrealInfo();
		PicoSDK::Render::RenderThreadInit();
		if (!GetDefault <UPicoMobileSettings>()->disableHmd6DofModule)
		{
			PicoSDK::Sensor::Enable6Dof(true);
		}
		PicoSDK::Sensor::SensorInit();
		PicoSDK::Sensor::StartSensor(PicoSDK::Sensor::GMainSensorIndex);

		EyeTracker->OpenEyeTracking();
		FPicoJava::HideLoading();
		PicoSDK::GIsSDKInit = true;
	}
	RefreshMainSensorState_RenderThread();
#endif
}

// IHeadMountedDisplay
EHMDWornState::Type FPicoMobileHMD::GetHMDWornState()
{
#if PLATFORM_ANDROID
	int32 pSensorStatus = -1;
	pSensorStatus = FPicoJava::Pvr_GetPsensorState();

	if (0 == pSensorStatus)
	{
		return EHMDWornState::Worn;
	}
	else if (1 <= pSensorStatus)
	{
		return EHMDWornState::NotWorn;
	}
	else
	{
		return EHMDWornState::Unknown;
	}
#endif
	return EHMDWornState::Unknown;
}

void FPicoMobileHMD::EnableHMD(bool enable)
{
}

bool FPicoMobileHMD::GetHMDMonitorInfo(MonitorInfo& MonitorDesc)
{
#if PLATFORM_ANDROID
	MonitorDesc.MonitorName = "";
	MonitorDesc.MonitorId = 0;
	MonitorDesc.DesktopX = MonitorDesc.DesktopY = MonitorDesc.ResolutionX = MonitorDesc.ResolutionY = 0;

	MonitorDesc.ResolutionX = PicoSDK::Render::GTextureSize.X;
	MonitorDesc.ResolutionY = PicoSDK::Render::GTextureSize.Y;

	return true;
#endif
	return false;
}

void FPicoMobileHMD::GetFieldOfView(float& OutHFOVInDegrees, float& OutVFOVInDegrees) const
{
#if PLATFORM_ANDROID
	OutVFOVInDegrees =  PicoSDK::Sensor::GVFov;
	OutHFOVInDegrees =  PicoSDK::Sensor::GHFov;
#endif
}

void FPicoMobileHMD::GetEyeRenderParams_RenderThread(const struct FRenderingCompositePassContext& Context, FVector2D& EyeToSrcUVScaleValue, FVector2D& EyeToSrcUVOffsetValue) const
{
	if (Context.View.StereoPass == eSSP_LEFT_EYE)
	{
		EyeToSrcUVOffsetValue.X = 0.0f;
		EyeToSrcUVOffsetValue.Y = 0.0f;

		EyeToSrcUVScaleValue.X = 0.5f;
		EyeToSrcUVScaleValue.Y = 1.0f;
	}
	else
	{
		EyeToSrcUVOffsetValue.X = 0.5f;
		EyeToSrcUVOffsetValue.Y = 0.0f;

		EyeToSrcUVScaleValue.X = 0.5f;
		EyeToSrcUVScaleValue.Y = 1.0f;
	}
}

void FPicoMobileHMD::SetInterpupillaryDistance(float NewInterpupillaryDistance)
{
}

float FPicoMobileHMD::GetInterpupillaryDistance() const
{
	float _ipd = 0.0f;
#if PLATFORM_ANDROID
	PicoSDK::Render::GetIPD(_ipd);
#endif
	//UE_LOG(LogPicoHMD, Log, TEXT("Pvr_UE Interpupillary Distance: %f"), _ipd);
	return _ipd;
}


bool FPicoMobileHMD::GetHMDDistortionEnabled(EShadingPath ShadingPath) const
{
	return false;
}


bool FPicoMobileHMD::IsChromaAbCorrectionEnabled() const
{
	return false;
}

float FPicoMobileHMD::GetPixelDenity() const
{
	return PixelDensity;
}

void FPicoMobileHMD::SetPixelDensity(const float NewPixelDensity)
{
	PixelDensity = NewPixelDensity;
	SetPicoRenderTargetSize();
}

bool FPicoMobileHMD::SetPicoRenderTargetSize()
{
#if PLATFORM_ANDROID
	UE_LOG(LogPicoHMD, Log, TEXT("RTScale = %f"), PixelDensity);
	FIntPoint IdealRTSize = GetIdealRenderTargetSize();
	if (bUsesArrayTexture)
	{
		PicoSDK::Render::GTextureSize.X = FMath::CeilToInt(static_cast<float>(IdealRTSize.X) * PixelDensity / 2);
	}
	else
	{
		PicoSDK::Render::GTextureSize.X = FMath::CeilToInt(static_cast<float>(IdealRTSize.X) * PixelDensity);
	}
	PicoSDK::Render::GTextureSize.Y = FMath::CeilToInt(static_cast<float>(IdealRTSize.Y) * PixelDensity);
	NormalizeRTSize();
	return true;
#endif 
	return false;
}

void FPicoMobileHMD::NormalizeRTSize()
{
#if PLATFORM_ANDROID
	const uint32 DividableBy = 4;
	const uint32 Mask = ~(DividableBy - 1);
	PicoSDK::Render::GTextureSize.X = (PicoSDK::Render::GTextureSize.X + DividableBy - 1) & Mask;
	PicoSDK::Render::GTextureSize.Y = (PicoSDK::Render::GTextureSize.Y + DividableBy - 1) & Mask;
	PicoSDK::Render::GTextureSize.X = PicoSDK::Render::GTextureSize.X > 4096 * 2 ? 4096 * 2 : PicoSDK::Render::GTextureSize.X;
	PicoSDK::Render::GTextureSize.Y = PicoSDK::Render::GTextureSize.Y > 4096 ? 4096 : PicoSDK::Render::GTextureSize.Y;
	UE_LOG(LogPicoHMD, Log, TEXT("NormalizeRTSize:GTextureSize %d,%d"), PicoSDK::Render::GTextureSize.X, PicoSDK::Render::GTextureSize.Y);
#endif
}

void FPicoMobileHMD::LimitFrameRate()
{
#if PLATFORM_ANDROID
	if (GEngine->bUseFixedFrameRate)
	{
		if (GEngine->FixedFrameRate > PicoSDK::Render::DisplayRate)
		{
			GEngine->FixedFrameRate = PicoSDK::Render::DisplayRate;
		}
	}
	if (GEngine->bSmoothFrameRate)
	{
		if (GEngine->SmoothedFrameRateRange.GetUpperBound().IsOpen())
		{
			GEngine->bUseFixedFrameRate = true;
			GEngine->FixedFrameRate = PicoSDK::Render::DisplayRate;
		}
		else
		{	
			if (GEngine->SmoothedFrameRateRange.GetUpperBoundValue() > PicoSDK::Render::DisplayRate)
			{
				GEngine->SmoothedFrameRateRange.SetUpperBoundValue(PicoSDK::Render::DisplayRate);
			}
		}
	}
	if (!GEngine->bSmoothFrameRate && !GEngine->bUseFixedFrameRate)
	{
		GEngine->bUseFixedFrameRate = true;
		GEngine->FixedFrameRate = PicoSDK::Render::DisplayRate;
	}
#endif
}

bool FPicoMobileHMD::GetMultiViewState()
{
	auto MobileMultiViewCVar = IConsoleManager::Get().FindTConsoleVariableDataInt(TEXT("vr.MobileMultiView"));
	bool bIsUsingMobileMultiView = GSupportsMobileMultiView && MobileMultiViewCVar && MobileMultiViewCVar->GetValueOnAnyThread() != 0;
	auto MobileMultiViewDirectCVar = IConsoleManager::Get().FindTConsoleVariableDataInt(TEXT("vr.MobileMultiView.Direct"));
	return bIsUsingMobileMultiView && MobileMultiViewDirectCVar && MobileMultiViewDirectCVar->GetValueOnAnyThread() != 0;
}

FIntPoint FPicoMobileHMD::GetIdealRenderTargetSize() const
{
	FIntPoint DefaultRTSize = FIntPoint(2048,1024);
#if PLATFORM_ANDROID
	PicoImport::PvrGetIntConfig(PicoSDKConfig::GlobalIntConfigs::EYE_TEXTURE_RESOLUTION0, DefaultRTSize.X);
	PicoImport::PvrGetIntConfig(PicoSDKConfig::GlobalIntConfigs::EYE_TEXTURE_RESOLUTION1, DefaultRTSize.Y);
	DefaultRTSize.X = DefaultRTSize.X * 2;
#endif
	return DefaultRTSize;
}

// IStereoRendering

bool FPicoMobileHMD::IsStereoEnabled() const
{
	return isStereoEnabled;
}

bool FPicoMobileHMD::EnableStereo(bool stereo)
{
	check(IsInGameThread());

	isStereoEnabled = true;

	return isStereoEnabled;
}

void FPicoMobileHMD::AdjustViewRect(EStereoscopicPass StereoPass, int32& X, int32& Y, uint32& SizeX, uint32& SizeY) const
{
#if PLATFORM_ANDROID
	if (bUsesArrayTexture)
	{
		SizeX = PicoSDK::Render::GTextureSize.X ;
	}
	else
	{
		SizeX = PicoSDK::Render::GTextureSize.X / 2;
	}
	
	SizeY = PicoSDK::Render::GTextureSize.Y;
	if (StereoPass == eSSP_RIGHT_EYE)
	{
		X += SizeX;
	}
	//UE_LOG(LogPicoHMD, Log, TEXT("Pvr_UE AdjustViewRect() : (StereoPass:%d ,X: %d,Y: %d ,SizeX: %d,SizeY: %d)"), (int)StereoPass, X, Y,SizeX, SizeY);

#endif
}

FMatrix FPicoMobileHMD::GetStereoProjectionMatrix(const EStereoscopicPass StereoPassType) const
{	
#if PLATFORM_ANDROID
	// SeeThrough boundary
	if(PicoSDK::SeeThrough::SeeThroughState == 2)
	{
		return FMatrix(
			FPlane(0.1f, 0.0f, 0.0f, 0.0f),
			FPlane(0.0f, 0.1f, 0.0f, 0.0f),
			FPlane(0.0f, 0.0f, 0.0f, 0.0f),
			FPlane(0.0f, 0.0f, 0.0f, 0.0f));
	}
	else
	{
		const float ProjectionCenterOffset = 0;// 0.151976421f;
		const float PassProjectionOffset = (StereoPassType == eSSP_LEFT_EYE) ? ProjectionCenterOffset : -ProjectionCenterOffset;
		const float HalfHFov = FMath::DegreesToRadians(PicoSDK::Sensor::GHFov) / 2.f;
		const float HalfVFov = FMath::DegreesToRadians(PicoSDK::Sensor::GVFov) / 2.f;
		const float Right = FPlatformMath::Tan(HalfHFov);
		const float Left = -FPlatformMath::Tan(HalfHFov);
		const float Bottom = -FPlatformMath::Tan(HalfVFov);
		const float Top = FPlatformMath::Tan(HalfVFov);

		float SumRL = (Right + Left);
		float SumTB = (Top + Bottom);
		float InvRL = (1.0f / (Right - Left));
		float InvTB = (1.0f / (Top - Bottom));
		const float InNearZ = GNearClippingPlane;
		return FMatrix(
			FPlane((2.0f * InvRL), 0.0f, 0.0f, 0.0f),
			FPlane(0.0f, (2.0f * InvTB), 0.0f, 0.0f),
			FPlane((SumRL * InvRL), (SumTB * InvTB), 0.0f, 1.0f),
			FPlane(0.0f, 0.0f, InNearZ, 0.0f))
			* FTranslationMatrix(FVector(PassProjectionOffset, 0, 0));
	}
#else 
	const float ProjectionCenterOffset = 0;// 0.151976421f;
	const float PassProjectionOffset = (StereoPassType == eSSP_LEFT_EYE) ? ProjectionCenterOffset : -ProjectionCenterOffset;
	const float HalfFov = FMath::DegreesToRadians(102.0f) / 2.f;
	const float InWidth = 1280.f;
	const float InHeight = 1280.f;
	const float XS = 1.0f / tan(HalfFov);
	const float YS = InWidth / tan(HalfFov) / InHeight;

	const float InNearZ = GNearClippingPlane;
	return FMatrix(
		FPlane(XS, 0.0f, 0.0f, 0.0f),
		FPlane(0.0f, YS, 0.0f, 0.0f),
		FPlane(0.0f, 0.0f, 0.0f, 1.0f),
		FPlane(0.0f, 0.0f, InNearZ, 0.0f))

		* FTranslationMatrix(FVector(PassProjectionOffset, 0, 0));	
#endif
}

uint32 FPicoMobileHMD::CreateLayer(const IStereoLayers::FLayerDesc & InLayerDesc)
{
	check(IsInGameThread());

	uint32 LayerId = NextPicoLayerId++;
	if (InLayerDesc.Texture != NULL)
	{
		UE_LOG(LogTemp, Warning, TEXT("Pvr_UE Add a Layer ID is %d "), LayerId);
		LayerSortIndex.Add(LayerId, -InLayerDesc.Priority);
		LayerSortIndex.ValueSort(TGreater<int32>());
		PicoLayerMap.Add(LayerId, MakeShareable(new FPicoVRStereoLayer(LayerId, InLayerDesc)));
	}
	
	return LayerId;
}

void FPicoMobileHMD::DestroyLayer(uint32 LayerId)
{
	check(IsInGameThread());
	LayerSortIndex.Remove(LayerId);
	PicoLayerMap.Remove(LayerId);
}

void FPicoMobileHMD::SetLayerDesc(uint32 LayerId, const IStereoLayers::FLayerDesc & InLayerDesc)
{
	/*check(IsInGameThread());

	FPicoVRStereoLayerPtr* LayerFound = PicoLayerMap.Find(LayerId);
	if (LayerFound)
	{
		FPicoVRStereoLayer* PicoLayer = new FPicoVRStereoLayer(**LayerFound);
		PicoLayer->SetDesc(InLayerDesc);
		*LayerFound = MakeShareable(PicoLayer);
	}*/
}

bool FPicoMobileHMD::GetLayerDesc(uint32 LayerId, IStereoLayers::FLayerDesc & OutLayerDesc)
{
	/*check(IsInGameThread());
	FPicoVRStereoLayerPtr* LayerFound = PicoLayerMap.Find(LayerId);

	if (LayerFound)
	{
		OutLayerDesc = (*LayerFound)->GetDesc();
		return true;
	}*/

	return false;
}

// ISceneViewExtension
void FPicoMobileHMD::SetupViewFamily(FSceneViewFamily& InViewFamily) 
{
#if WITH_EDITOR
	InViewFamily.EngineShowFlags.SetScreenPercentage(false);
#endif
}

void FPicoMobileHMD::SetupView(FSceneViewFamily& InViewFamily, FSceneView& InView){}

void FPicoMobileHMD::BeginRenderViewFamily(FSceneViewFamily& InViewFamily) {}

void FPicoMobileHMD::PreRenderViewFamily_RenderThread(FRHICommandListImmediate& RHICmdList, FSceneViewFamily& ViewFamily){}

void FPicoMobileHMD::PreRenderView_RenderThread(FRHICommandListImmediate& RHICmdList, FSceneView& InView){}

void FPicoMobileHMD::PostRenderViewFamily_RenderThread(FRHICommandListImmediate& RHICmdList, FSceneViewFamily& InViewFamily)
{
	check(IsInRenderingThread());

	OverlayRendering();

#if PLATFORM_ANDROID
	PicoSDK::SeeThrough::SetReinPosition();
#endif
	//FinishRendering();
}

// IStereoRenderTargetManager
bool FPicoMobileHMD::ShouldUseSeparateRenderTarget() const
{
	check(IsInGameThread());
	return true;// IsStereoEnabled();
}

void FPicoMobileHMD::UpdateViewport(bool bUseSeparateRenderTarget, const FViewport & Viewport, SViewport * ViewportWidget) 
{
	
}

void FPicoMobileHMD::CalculateRenderTargetSize(const class FViewport& Viewport, uint32& InOutSizeX, uint32& InOutSizeY)
{
	check(IsInGameThread());
#if PLATFORM_ANDROID
	InOutSizeX = PicoSDK::Render::GTextureSize.X;
	InOutSizeY = PicoSDK::Render::GTextureSize.Y;
#endif
}

bool FPicoMobileHMD::NeedReAllocateViewportRenderTarget(const FViewport& Viewport)
{
	check(IsInGameThread());

	const uint32 InSizeX = Viewport.GetSizeXY().X;
	const uint32 InSizeY = Viewport.GetSizeXY().Y;

	FIntPoint RTSize;
	RTSize.X = Viewport.GetRenderTargetTexture()->GetSizeX();
	RTSize.Y = Viewport.GetRenderTargetTexture()->GetSizeY();

	uint32 NewSizeX = InSizeX, NewSizeY = InSizeY;
	CalculateRenderTargetSize(Viewport, NewSizeX, NewSizeY);
	if (NewSizeX != RTSize.X || NewSizeY != RTSize.Y)
	{
		return true;
	}
	return false;
}

// FXRTrackingSystemBase
float FPicoMobileHMD::GetWorldToMetersScale() const
{
	if (IsInGameThread() && GWorld != nullptr)
	{
		return GWorld->GetWorldSettings()->WorldToMeters;
	}

	// Default value, assume Unreal units are in centimeters
	return 100.0f;
}

bool FPicoMobileHMD::IsInitialized() const
{
	return true;
}

void FPicoMobileHMD::RefreshMainSensorState_RenderThread()
{
#if PLATFORM_ANDROID
	check(IsInRenderingThread());

	PicoSDK::Sensor::GetMainSensorState(true);

	// SeeThrough boundary
	PicoSDK::SeeThrough::SeeThroughState = PicoSDK::SeeThrough::GetSeeThroughState();
	if (PicoSDK::SeeThrough::SeeThroughState == 2 && PicoImport::PvrGetFrameRateLimit())
	{
		if (GEngine->bUseFixedFrameRate && GEngine->FixedFrameRate != 30.101f)
		{
			UseFixedFrameRate = true;
			FixedFrameRate = GEngine->FixedFrameRate;
			GEngine->FixedFrameRate = 30.101f;
		}
		else
		{
			GEngine->bUseFixedFrameRate = true;
			GEngine->FixedFrameRate = 30.101f;
			UseFixedFrameRate = false;
		}
	}
	else
	{
		if (GEngine->bUseFixedFrameRate && GEngine->FixedFrameRate == 30.101f)
		{
			GEngine->bUseFixedFrameRate = UseFixedFrameRate;
			GEngine->FixedFrameRate = FixedFrameRate;
		}
	}

#endif
}

void FPicoMobileHMD::ConnectToAndroid()
{
#if PLATFORM_ANDROID

	PicoImport::PvrSetInitActivity(FAndroidApplication::GetGameActivityThis(), FPicoJava::GetRawClass());

#endif
}

void FPicoMobileHMD::SubmitUnrealInfo()
{
#if PLATFORM_ANDROID
	// to notificate so Unreal Version
	UE_LOG(LogPicoHMD, Warning, TEXT("Pvr_UE Unreal Version: %d "), (int)(ENGINE_MINOR_VERSION));
	PicoSDK::Render::PvrSetUnrealParam((int32)UnrealParamKeys::UNREAL_VERSION, (int32)ENGINE_MINOR_VERSION);
	
	// to notificate so builf type
#if UE_BUILD_DEBUG 
	UE_LOG(LogPicoHMD, Warning, TEXT("Pvr_UE UE_BUILD_DEBUG 0 "));
	PicoSDK::Render::PvrSetUnrealParam((int32)UnrealParamKeys::BUILD_TYPE, 0);
#endif
#if UE_BUILD_DEVELOPMENT
	UE_LOG(LogPicoHMD, Warning, TEXT("Pvr_UE UE_BUILD_DEVELOPMENT 1 "));
	PicoSDK::Render::PvrSetUnrealParam((int32)UnrealParamKeys::BUILD_TYPE, 1);
#endif
#if UE_BUILD_SHIPPING
	UE_LOG(LogPicoHMD, Warning, TEXT("Pvr_UE UE_BUILD_SHIPPING 2 "));
	PicoSDK::Render::PvrSetUnrealParam((int32)UnrealParamKeys::BUILD_TYPE, 2);
#endif
#endif
}

bool FPicoMobileHMD::Startup()
{
#if PLATFORM_ANDROID
	bool ret = PicoImport::Init();// Import so
	check(ret);

	IConsoleVariable* CVar = IConsoleManager::Get().FindConsoleVariable(TEXT("r.MobileContentScaleFactor"));
	CVar->Set(0.0f);

	ConnectToAndroid();// Init activity

	FCoreDelegates::ApplicationWillEnterBackgroundDelegate.AddRaw(this, &FPicoMobileHMD::ApplicationPauseDelegate);
	FCoreDelegates::ApplicationHasEnteredForegroundDelegate.AddRaw(this, &FPicoMobileHMD::ApplicationResumeDelegate);

	PicoSplash = MakeShareable(new FPicoVRSplash(this));
	PicoSplash->RegisterForMapLoad();
	
	if (GetMultiViewState())
	{
		bUsesArrayTexture = true;
	}
	static const auto PixelDensityCVar = IConsoleManager::Get().FindConsoleVariable(TEXT("vr.PixelDensity"));
	SetPixelDensity(FMath::Clamp(PixelDensityCVar->GetFloat(), 0.1f, 2.0f));
	PicoImport::PvrSubmitRenderTargetSize(PicoSDK::Render::GTextureSize.X, PicoSDK::Render::GTextureSize.Y);

	//PicoSDK::Render::GetTextureSize(PicoSDK::Render::GTextureSize);
	PicoSDK::Render::GetFOV(PicoSDK::Sensor::GFov);
	PicoSDK::Render::GetFOV(PicoSDK::Sensor::GVFov,PicoSDK::Sensor::GHFov);
	PicoSDK::Sensor::GetSensorCount(PicoSDK::Sensor::GSensorCount);
	PicoSDK::Sensor::GetNeckOffset();
	
	IConsoleVariable* CVSyns = IConsoleManager::Get().FindConsoleVariable(TEXT("r.VSync"));
	CVSyns->Set(0.0f);
	IConsoleVariable* CFCFVar = IConsoleManager::Get().FindConsoleVariable(TEXT("r.FinishCurrentFrame"));
	CFCFVar->Set(0.0f);
	PicoSDK::Render::GetDisplayRefreshRate(PicoSDK::Render::DisplayRate);
	LimitFrameRate();

	PicoSDK::Render::FFRLevel = (int32)GetDefault <UPicoMobileSettings>()->FFRLevel;
	PicoSDK::Render::EnableMultiView(bUsesArrayTexture);
	return ret;
#endif
	return false;
}

void FPicoMobileHMD::Shutdown()
{
#if PLATFORM_ANDROID
	// Restore intervention for home key key injection
	// TODO:You should put it in Activity::onDestroy(), but there's no call underlying here, and there's an assertion that can't be executed to onDestroy();
	JNIEnv* Env = FAndroidApplication::GetJavaEnv();

	FJavaWrapper::CallVoidMethod(
		Env,
		FJavaWrapper::GameActivityThis,
		FJavaWrapper::FindMethod(
			Env,
			FJavaWrapper::GameActivityClassID,
			"resetHomeKey",
			"()V",
			false)
	);
	if (IsInRenderingThread())
	{
		if (PicoSDK::GIsSDKInit)
		{
			PicoSDK::Sensor::StopSensor(PicoSDK::Sensor::GMainSensorIndex);
			
			PicoSDK::Render::RenderThreadShutdown();
			check(GJavaVM);
			GJavaVM->DetachCurrentThread();
		}

		PicoImport::Uninit();
	}
	else
	{
#if ENGINE_MINOR_VERSION <= 21
		ENQUEUE_UNIQUE_RENDER_COMMAND(
			Shutdown,
#else
		FPicoMobileHMD * const Self = this;
		ENQUEUE_RENDER_COMMAND(LeaveVRModeCmd)([Self](FRHICommandListImmediate& RHICmdList)
#endif
			{
				if (PicoSDK::GIsSDKInit)
				{
					PicoSDK::Sensor::StopSensor(PicoSDK::Sensor::GMainSensorIndex);

					PicoSDK::Render::RenderThreadShutdown();
					check(GJavaVM);
					GJavaVM->DetachCurrentThread();
				}

				PicoImport::Uninit();
			}
		);

		FlushRenderingCommands();
	}
#endif
}

void FPicoMobileHMD::ApplicationPauseDelegate()
{
#if PLATFORM_ANDROID
	FPlatformMisc::LowLevelOutputDebugStringf(TEXT("DDD UE4 PicoVR PLUGIN PAUSE, tid = %d"), gettid());
	FPicoJava::CurrentGameStatus = GamePaused;
	if (PicoSDK::GIsSDKInit)
	{
		LeaveVRMode();
	}
	EyeTracker->bEyeTrackingRun = false;
	FPlatformMisc::LowLevelOutputDebugStringf(TEXT("UE4 PicoVR PLUGIN PAUSE END, tid = %d"), gettid());
#endif
}

void FPicoMobileHMD::ApplicationResumeDelegate()
{
#if PLATFORM_ANDROID
	FPlatformMisc::LowLevelOutputDebugStringf(TEXT("DDD UE4 PicoVR PLUGIN RESUME, tid = %d"), gettid());

	FPicoJava::CurrentGameStatus = GameResumed;

	// Safe Panel
	if (!GetDefault <UPicoMobileSettings>()->disableHmd6DofModule)
	{
		FPicoJava::IsHead6dofReset();
	}

	if (PicoSDK::GIsSDKInit)
	{
		FPlatformProcess::Sleep(1.0f);
		EnterVRMode();
		EyeTracker->OpenEyeTracking();
	}
	

	FPlatformMisc::LowLevelOutputDebugStringf(TEXT("UE4 PicoVR PLUGIN RESUME END, tid = %d"), gettid());
#endif
}

void FPicoMobileHMD::EnterVRMode()
{
#if PLATFORM_ANDROID
	if (IsInRenderingThread())
	{
		FPlatformMisc::LowLevelOutputDebugStringf(TEXT("UE4 PicoVR PLUGIN EnterVRMode, On RT! tid = %d"), gettid());

		PicoSDK::Sensor::StartSensor(PicoSDK::Sensor::GMainSensorIndex);
		PicoSDK::SendResumeEvent();
	}
	else
	{
		FPlatformMisc::LowLevelOutputDebugStringf(TEXT("UE4 PicoVR PLUGIN EnterVRMode, tid = %d"), gettid());

#if ENGINE_MINOR_VERSION <= 21
		ENQUEUE_UNIQUE_RENDER_COMMAND(
			EnterVRMode,
#else
		FPicoMobileHMD * const Self = this;
		ENQUEUE_RENDER_COMMAND(EnterVRModeCmd)([Self](FRHICommandListImmediate& RHICmdList)
#endif
			{
				PicoSDK::Sensor::StartSensor(PicoSDK::Sensor::GMainSensorIndex);
				PicoSDK::SendResumeEvent();
            }
		);

		FlushRenderingCommands();
	}

	FPlatformMisc::LowLevelOutputDebugStringf(TEXT("UE4 PicoVR PLUGIN EnterVRMode END, tid = %d"), gettid());
#endif
}

void FPicoMobileHMD::LeaveVRMode()
{
#if PLATFORM_ANDROID
	if (IsInRenderingThread())
	{
		FPlatformMisc::LowLevelOutputDebugStringf(TEXT("UE4 PicoVR PLUGIN LeaveVRMode, On RT! tid = %d"), gettid());

		PicoSDK::SendPauseEvent();
		PicoSDK::Sensor::StopSensor(PicoSDK::Sensor::GMainSensorIndex);
	}
	else
	{
		FPlatformMisc::LowLevelOutputDebugStringf(TEXT("UE4 PicoVR PLUGIN LeaveVRMode, tid = %d"), gettid());

#if ENGINE_MINOR_VERSION <= 21
		ENQUEUE_UNIQUE_RENDER_COMMAND(
			LeaveVRMode,
#else
		FPicoMobileHMD * const Self = this;
		ENQUEUE_RENDER_COMMAND(LeaveVRModeCmd)([Self](FRHICommandListImmediate& RHICmdList)
#endif
			{
				PicoSDK::SendPauseEvent();
		        PicoSDK::Sensor::StopSensor(PicoSDK::Sensor::GMainSensorIndex);
			}
		);

		FlushRenderingCommands();
	}

	FPlatformMisc::LowLevelOutputDebugStringf(TEXT("UE4 PicoVR PLUGIN LeaveVRMode END, tid = %d"), gettid());
#endif
}

void FPicoMobileHMD::OverlayRendering()
{
#if PLATFORM_ANDROID
	if (PicoSDK::GIsSDKInit && LayerSortIndex.Num() > 0 && PicoSDK::SeeThrough::SeeThroughState != 2)
	{
		FPicoVRStereoLayerPtr* LayerFound = nullptr;
		int32 layerIndex = 0;
		for (TPair<uint32, int32>& element : LayerSortIndex)
		{
			if (element.Key != (LayerSortIndex.Num() - 1))
			{
				LayerFound = PicoLayerMap.Find(element.Key);
				if (LayerFound)
				{
					GLuint RTexID = (*LayerFound)->GetRTexID();
					GLuint LTexID = (*LayerFound)->GetLTexID();

					FQuat CamRotation = PicoSDK::Sensor::GCurrentOrientation;
					FVector CamLocation = PicoSDK::Sensor::GCurrentPosition + InitCamPos;
					FMatrix mMVMatrix = (*LayerFound)->GetLayerMV(CamRotation, CamLocation, isFixOverlay);

					float mMVFloat[16];
					int l = 0;
					for (int32 i = 0; i < 4; i++) {
						for (int j = 0; j < 4; j++) {
							mMVFloat[l] = mMVMatrix.M[i][j];
							l++;
						}
					}

					float scaleM[3] = { (*LayerFound)->GetLayerPos().X,(*LayerFound)->GetLayerPos().Y ,(*LayerFound)->GetLayerPos().Z };
					//memset(scaleM,1,3*sizeof(float));
					float rotationM[4] = { (*LayerFound)->GetLayerRot().X,(*LayerFound)->GetLayerRot().Y ,(*LayerFound)->GetLayerRot().Z ,(*LayerFound)->GetLayerRot().W };
					//memset(rotationM, 0, 4 * sizeof(float));
					float translationM[3] = { (*LayerFound)->GetLayerSca().X,(*LayerFound)->GetLayerSca().Y ,(*LayerFound)->GetLayerSca().Z };
					//memset(translationM, 0, 3 * sizeof(float));
					float rotationC[4] = { CamRotation.X,CamRotation.Y,CamRotation.Z,CamRotation.W };
					//memset(rotationC, 0, 4 * sizeof(float));
					float translationC[3] = { CamLocation.X,CamLocation.Y,CamLocation.Z };
					//memset(translationC, 0, 3 * sizeof(float));

					layerIndex++;
					//layerType imageType texid eye layerindex fixedoverlay mv 
					PicoImport::PvrSetOverlayModelViewMatrix(0, 0, RTexID, 1, layerIndex, isFixOverlay, 0, mMVFloat, scaleM, rotationM, translationM, rotationC, translationC);
					PicoImport::PvrSetOverlayModelViewMatrix(0, 0, LTexID, 0, layerIndex, isFixOverlay, 0, mMVFloat, scaleM, rotationM, translationM, rotationC, translationC);
				}
			}
		}
	}
#endif
}


#undef LOCTEXT_NAMESPACE
