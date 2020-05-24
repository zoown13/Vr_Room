// Copyright Pico Technology Co, Ltd. All Rights Reserved.

#include "PicoImport.h"
#include "PicoJava.h"
#include "../Launch/Resources/Version.h"
#include "PicoMobileSettings.h"

DEFINE_LOG_CATEGORY_STATIC(LogPicoImport, Log, All);

#if PLATFORM_ANDROID
#include "Android/AndroidWindow.h"

#define DLL_IMPORT(Name, Func) \
	Func = (FDll##Func##Proc)dlsym(PicoImport::GSharedLibraryHandle, #Name); \
	if (Func == nullptr) \
	{ \
		UE_LOG(LogPicoImport, Display, TEXT("GetDllExport  %s Failed! "),#Name);\
		return false; \
	}

#define DLL_DEFINE(Func) \
	FDll##Func##Proc PicoImport::Func = nullptr

void* PicoImport::GSharedLibraryHandle = nullptr;
FString PicoImport::GPluginDir;


DLL_DEFINE(PvrInit);
DLL_DEFINE(PvrStartSensor);
DLL_DEFINE(PvrStopSensor);
DLL_DEFINE(PvrResetSensor);
DLL_DEFINE(PvrOptionalResetSensor);
DLL_DEFINE(PvrGetMainSensorState);
DLL_DEFINE(PvrGetSensorState);
DLL_DEFINE(PvrGetPsensorState);
DLL_DEFINE(PvrGetAcceleration);
DLL_DEFINE(PvrGetAngularVelocity);
DLL_DEFINE(PvrGetIntConfig);
DLL_DEFINE(PvrGetFloatConfig);
DLL_DEFINE(PvrChangeScreenParameters);
DLL_DEFINE(PvrSetInitActivity);
DLL_DEFINE(PvrGetSDKVersion);
DLL_DEFINE(PvrGetGpuUtilization);
DLL_DEFINE(PvrGetSeeThroughState);
DLL_DEFINE(PvrSubmitRenderTargetSize);
DLL_DEFINE(PvrSetTrackingOrigin);
DLL_DEFINE(PvrGetFrameRateLimit);
DLL_DEFINE(PvrUnityRenderEvent);
DLL_DEFINE(PvrSetOverlayModelViewMatrix);
DLL_DEFINE(PvrEnable6DofModule);
DLL_DEFINE(PvrSetFoveationParameters);
DLL_DEFINE(PvrGetEyeTrackingData);
DLL_DEFINE(PvrGetTrackingMode);
DLL_DEFINE(PvrSetTrackingMode);
DLL_DEFINE(PvrSetUnrealParam);
DLL_DEFINE(PvrSetCPUGPULevels);
DLL_DEFINE(PvrEnableMultiView);
DLL_DEFINE(PvrGetBoundaryConfigured);
DLL_DEFINE(PvrGetBoundaryEnabled);
DLL_DEFINE(PvrSetBoundaryVisible);
DLL_DEFINE(PvrSetReinPosition);

FString PicoImport::GetPluginDir()
{
	return GPluginDir;
}

bool PicoImport::Init()
{
	GSharedLibraryHandle = dlopen("libPvr_UESDK.so", 0);
	if (GSharedLibraryHandle != NULL)
	{
		PvrInit = (FDllPvrInitProc)dlsym(GSharedLibraryHandle, "Pvr_Init");
		if (!PvrInit) UE_LOG(LogPicoImport, Warning, TEXT("Pvr_Init not found!"));

		PvrStartSensor = (FDllPvrStartSensorProc)dlsym(GSharedLibraryHandle, "Pvr_StartSensor");
		if (!PvrStartSensor) UE_LOG(LogPicoImport, Warning, TEXT("PvrStartSensor not found!"));

		PvrStopSensor = (FDllPvrStopSensorProc)dlsym(GSharedLibraryHandle, "Pvr_StopSensor");
		if (!PvrStopSensor) UE_LOG(LogPicoImport, Warning, TEXT("PvrStopSensor not found!"));

		PvrResetSensor = (FDllPvrResetSensorProc)dlsym(GSharedLibraryHandle, "Pvr_ResetSensor");
		if (!PvrResetSensor) UE_LOG(LogPicoImport, Warning, TEXT("PvrResetSensor not found!"));

		PvrOptionalResetSensor = (FDllPvrOptionalResetSensorProc)dlsym(GSharedLibraryHandle, "Pvr_OptionalResetSensor");
		if (!PvrOptionalResetSensor) UE_LOG(LogPicoImport, Warning, TEXT("PvrOptionalResetSensor not found!"));

		PvrGetSensorState = (FDllPvrGetSensorStateProc)dlsym(GSharedLibraryHandle, "Pvr_GetSensorState");
		if (!PvrGetSensorState) UE_LOG(LogPicoImport, Warning, TEXT("PvrGetSensorState not found!"));

		PvrGetMainSensorState = (FDllPvrGetMainSensorStateProc)dlsym(GSharedLibraryHandle, "Pvr_GetMainSensorState");
		if (!PvrGetMainSensorState) UE_LOG(LogPicoImport, Warning, TEXT("PvrGetMainSensorState not found!"));

		PvrGetPsensorState = (FDllPvrGetPsensorStateProc)dlsym(GSharedLibraryHandle, "Pvr_GetPsensorState");
		if (!PvrGetPsensorState) UE_LOG(LogPicoImport, Warning, TEXT("PvrGetPsensorState not found!"));

		PvrGetIntConfig = (FDllPvrGetIntConfigProc)dlsym(GSharedLibraryHandle, "Pvr_GetIntConfig");
		if (!PvrGetIntConfig) UE_LOG(LogPicoImport, Warning, TEXT("PvrGetIntConfig not found!"));

		PvrGetFloatConfig = (FDllPvrGetFloatConfigProc)dlsym(GSharedLibraryHandle, "Pvr_GetFloatConfig");
		if (!PvrGetFloatConfig) UE_LOG(LogPicoImport, Warning, TEXT("PvrGetFloatConfig not found!"));

		PvrChangeScreenParameters = (FDllPvrChangeScreenParametersProc)dlsym(GSharedLibraryHandle, "Pvr_ChangeScreenParameters");
		if (!PvrChangeScreenParameters) UE_LOG(LogPicoImport, Warning, TEXT("PvrChangeScreenParameters not found!"));

		PvrSetInitActivity = (FDllPvrSetInitActivityProc)dlsym(GSharedLibraryHandle, "Pvr_SetInitActivity");
		if (!PvrSetInitActivity) UE_LOG(LogPicoImport, Warning, TEXT("PvrSetInitActivity not found!"));

		PvrGetSDKVersion = (FDllPvrGetSDKVersionProc)dlsym(GSharedLibraryHandle, "Pvr_GetSDKVersion");
		if (!PvrSetInitActivity) UE_LOG(LogPicoImport, Warning, TEXT("PvrGetSDKVersion not found!"));

		PvrGetGpuUtilization = (FDllPvrGetGpuUtilizationProc)dlsym(GSharedLibraryHandle, "PVR_GetGpuUtilization");
		if (!PvrGetGpuUtilization) UE_LOG(LogPicoImport, Warning, TEXT(" PVR_GetGpuUtilization not found!"));

		PvrGetSeeThroughState = (FDllPvrGetSeeThroughStateProc)dlsym(GSharedLibraryHandle, "Pvr_GetSeeThroughState");
		if (!PvrGetSeeThroughState) UE_LOG(LogPicoImport, Warning, TEXT(" PvrGetSeeThroughState not found!"));

		PvrSubmitRenderTargetSize = (FDllPvrSubmitRenderTargetSizeProc)dlsym(GSharedLibraryHandle, "Pvr_SetViewportSize");
		if (!PvrSubmitRenderTargetSize) UE_LOG(LogPicoImport, Warning, TEXT(" PvrSubmitRenderTargetSize not found!"));

		PvrSetTrackingOrigin = (FDllPvrSetTrackingOriginProc)dlsym(GSharedLibraryHandle, "Pvr_SetTrackingOrigin");
		if (!PvrSetTrackingOrigin) UE_LOG(LogPicoImport, Warning, TEXT("PvrSetTrackingOrigin not found!"));

		PvrGetFrameRateLimit = (FDllPvrGetFrameRateLimitProc)dlsym(GSharedLibraryHandle, "Pvr_GetFrameRateLimit");
		if (!PvrGetFrameRateLimit) UE_LOG(LogPicoImport, Warning, TEXT("PvrGetFrameRateLimit not found!"));
		
		PvrGetBoundaryConfigured = (FDllPvrGetBoundaryConfiguredProc)dlsym(GSharedLibraryHandle, "Pvr_BoundaryGetConfigured");
		if (!PvrGetBoundaryConfigured) UE_LOG(LogPicoImport, Warning, TEXT("PvrBoundaryGetConfigured not found!"));

		PvrGetBoundaryEnabled = (FDllPvrGetBoundaryEnabledProc)dlsym(GSharedLibraryHandle, "Pvr_BoundaryGetEnabled");
		if (!PvrGetBoundaryEnabled) UE_LOG(LogPicoImport, Warning, TEXT("PvrBoundaryGetEnabled not found!"));

		PvrSetBoundaryVisible = (FDllPvrSetBoundaryVisibleProc)dlsym(GSharedLibraryHandle, "Pvr_BoundarySetVisible");
		if (!PvrSetBoundaryVisible) UE_LOG(LogPicoImport, Warning, TEXT("PvrBoundarySetVisible not found!"));

		PvrSetReinPosition = (FDllPvrSetReinPositionProc)dlsym(GSharedLibraryHandle, "Pvr_SetReinPosition");
		if (!PvrSetReinPosition) UE_LOG(LogPicoImport, Warning, TEXT("PvrSetReinPosition not found!"));

		PvrUnityRenderEvent = (FDllPvrUnityRenderEventProc)dlsym(GSharedLibraryHandle, "UnityRenderEvent");
		if (!PvrUnityRenderEvent) UE_LOG(LogPicoImport, Warning, TEXT("PvrUnityRenderEvent not found!"));

		PvrSetOverlayModelViewMatrix = (FDllPvrSetOverlayModelViewMatrixProc)dlsym(GSharedLibraryHandle, "Pvr_SetOverlayModelViewMatrix_");
		if (!PvrSetOverlayModelViewMatrix) UE_LOG(LogPicoImport, Warning, TEXT("PvrSetOverlayModelViewMatrix not found!"));

		PvrEnable6DofModule = (FDllPvrEnable6DofModuleProc)dlsym(GSharedLibraryHandle, "Pvr_Enable6DofModule");
		if (!PvrEnable6DofModule) UE_LOG(LogPicoImport, Warning, TEXT("PvrEnable6DofModule not found!"));

		PvrSetFoveationParameters = (FDllPvrSetFoveationParametersProc)dlsym(GSharedLibraryHandle, "Pvr_SetFoveationParameters");
		if (!PvrSetFoveationParameters) UE_LOG(LogPicoImport, Warning, TEXT("PvrSetFoveationParameters not found!"));

		PvrGetEyeTrackingData = (FDllPvrGetEyeTrackingDataProc)dlsym(GSharedLibraryHandle, "Pvr_GetEyeTrackingData");
		if (!PvrGetEyeTrackingData)UE_LOG(LogPicoImport, Warning, TEXT("PvrGetEyeTrackingData not found!"));
		
		PvrGetTrackingMode = (FDllPvrGetTrackingModeProc)dlsym(GSharedLibraryHandle, "Pvr_GetTrackingMode");
		if (!PvrGetTrackingMode)UE_LOG(LogPicoImport, Warning, TEXT("PvrGetTrackingMode not found!"));

		PvrSetTrackingMode = (FDllPvrSetTrackingModeProc)dlsym(GSharedLibraryHandle, "Pvr_SetTrackingMode");
		if (!PvrSetTrackingMode)UE_LOG(LogPicoImport, Warning, TEXT("PvrSetTrackingMode not found!"));

		PvrSetUnrealParam = (FDllPvrSetUnrealParamProc)dlsym(GSharedLibraryHandle, "PVR_SetUnrealParam");
		if (!PvrSetUnrealParam) UE_LOG(LogPicoImport, Warning, TEXT("PVR_SetUnrealParam not found!"));
		
		PvrSetCPUGPULevels = (FDllPvrSetCPUGPULevelsProc)dlsym(GSharedLibraryHandle, "PVR_setPerformanceLevels");
		if (!PvrSetCPUGPULevels) UE_LOG(LogPicoImport, Warning, TEXT("PVR_setPerformanceLevels not found!"));
		
		PvrEnableMultiView = (FDllPvrEnableMultiViewProc)dlsym(GSharedLibraryHandle, "Pvr_EnableSinglePass");
		if (!PvrEnableMultiView) UE_LOG(LogPicoImport, Warning, TEXT("PvrEnableMultiView not found!"));



		return true;
	}

	return false;
}

void PicoImport::Uninit()
{
	GPluginDir.Empty();

	FreeDependency(GSharedLibraryHandle);
}


void PicoImport::FreeDependency(void*& Handle)
{
	if (Handle != nullptr)
	{
		dlclose(Handle);
		Handle = nullptr;
	}
}

bool PicoImport::LoadDependency(const FString& Dir, const FString& Name, void*& Handle)
{
	return true;
}


bool PicoSDK::GIsSDKInit = false;
bool PicoSDK::GIsMapPost = false;
uint32 PicoSDK::GMapInitFrame = 1;


int32 PicoSDK::Sensor::GMainSensorIndex = 0;
int32 PicoSDK::Sensor::GSensorCount = 0;
bool PicoSDK::Sensor::GSensorStart[] = { false, false};
bool PicoSDK::Sensor::GSensorInit = false;
bool PicoSDK::Sensor::GSensorCanUse = false;
float PicoSDK::Sensor::GFov = 90;
float PicoSDK::Sensor::GVFov = 90;
float PicoSDK::Sensor::GHFov = 90;
FQuat PicoSDK::Sensor::GCurrentOrientation = FQuat::Identity;
FVector PicoSDK::Sensor::GCurrentPosition = FVector::ZeroVector;
int32 PicoSDK::Sensor::GViewnumber = 0;
bool PicoSDK::Sensor::GIs6DofEnabled = false;
bool PicoSDK::Sensor::EnableNeckModel = false;
FVector PicoSDK::Sensor::NeckOffset = FVector::ZeroVector;
int32 PicoSDK::Sensor::TrackingOrigin = 0;

//FFR
GLuint PicoSDK::Render::PreviousId = 0;

#define CHECK_SENSOR_INDEX(index) \
if( (index < 0) ||  (index > PicoSDK::Sensor::GSensorCount - 1) || (index > MAX_SENSOR_COUNT - 1) )\
{\
UE_LOG(LogPicoImport, Display, TEXT("Sensor index out of range, please checkout max index using GetSensorCount()!"));\
return false;\
}


bool PicoSDK::Sensor::SensorInit()
{
	bool enable = false;
	if (PicoImport::PvrInit((int32)GMainSensorIndex) == 0)
	{
		GSensorInit = true;
		enable = true;
		UE_LOG(LogPicoImport, Display, TEXT("Pvr_UE PicoMobile PvrInit Success!"));
	}
	else
		UE_LOG(LogPicoImport, Display, TEXT("Pvr_UE PicoMobile PvrInit Failed!"));
	return enable;
}

bool PicoSDK::Sensor::GetSensorCount(int32& count)
{
	bool retB = false;
	int32 _sensorCount = 0;
	PicoImport::PvrGetIntConfig(PicoSDKConfig::GlobalIntConfigs::SEENSOR_COUNT, _sensorCount);

	if (_sensorCount >= 1)
	{
		UE_LOG(LogPicoImport, Display, TEXT("Pvr_UE PicoMobile GetSensorCount Success!"));
		retB = true;
		count = _sensorCount;
	}
	else
		UE_LOG(LogPicoImport, Display, TEXT("Pvr_UE PicoMobile GetSensorCount Failed!"));

	return retB;

}

bool PicoSDK::Sensor::StartSensor(int32 sensorIndex)
{
	CHECK_SENSOR_INDEX(sensorIndex);

	bool ret = false;

	if (GSensorStart[sensorIndex])
	{
		UE_LOG(LogPicoImport, Display, TEXT("Pvr_UE Sensor %d already started!"), sensorIndex);
		return ret;
	}

	if (PicoImport::PvrStartSensor(sensorIndex) == 0)
	{
		UE_LOG(LogPicoImport, Display, TEXT("Pvr_UE PicoMobile StartSensor %d Success!"), sensorIndex);
		ret = true;
		GSensorStart[sensorIndex] = true;
	}
	else
		UE_LOG(LogPicoImport, Display, TEXT("Pvr_UE PicoMobile StartSensor %d Failed!"), sensorIndex);
	return ret;
}

bool PicoSDK::Sensor::StopSensor(int32 sensorIndex)
{

	CHECK_SENSOR_INDEX(sensorIndex);

	bool ret = false;

	if (GSensorStart[sensorIndex])
	{
		if (PicoImport::PvrStopSensor(sensorIndex) == 0)
		{
			GSensorStart[sensorIndex] = false;
			ret = true;
		}
	}
	else
	{
		UE_LOG(LogPicoImport, Display, TEXT("Pvr_UE Sensor %d was not started!"), sensorIndex);
	}
	return ret;

}

bool PicoSDK::Sensor::ResetSensor(int32 sensorIndex)
{
	CHECK_SENSOR_INDEX(sensorIndex);

	bool ret = false;
	if (GSensorStart[sensorIndex])
	{
		if (PicoImport::PvrResetSensor(sensorIndex) == 0)
			ret = true;
	}
	return ret;
}

bool PicoSDK::Sensor::OptionalResetSensor(int32 sensorIndex, int32 isResetRotation, int32 isResetPosition)
{
	CHECK_SENSOR_INDEX(sensorIndex);

	bool ret = false;
	if (GSensorStart[sensorIndex])
	{
		if (PicoImport::PvrOptionalResetSensor(sensorIndex, isResetRotation, isResetPosition))
		{
			ret = true;
		}
	}
	return ret;
}

bool PicoSDK::Sensor::GetSensorState(int32 sensorIndex, FQuat& orientation, FVector& position)
{
	CHECK_SENSOR_INDEX(sensorIndex);

	if (sensorIndex == PicoSDK::Sensor::GMainSensorIndex)
	{
#ifdef UE_BUILD_DEBUG
		UE_LOG(LogPicoImport, Display, TEXT("Pvr_UE Sensor index error, please use this only for handheld device(aka Pico Neo box)!"));
#endif // UE_BUILD_DEBUG
		return false;
	}

	bool retB = false;
	if (GSensorStart[sensorIndex])
	{
		float w = 0, x = 0, y = 0, z = 0, px = 0, py = 0, pz = 0;
		int32 ret = PicoImport::PvrGetSensorState(sensorIndex, w, x, y, z, px, py, pz);
		if (ret == 0)
		{
			orientation = FQuat(z, -x, -y, w);
			position = FVector(-pz * 100, px * 100, py * 100);
			retB = true;
		}
		if (ret == -1)
			retB = false;

	}
	return retB;
}

bool PicoSDK::Sensor::GetMainSensorState(bool isInRenderThread)
{
	bool enable = false;
	if (GSensorStart[GMainSensorIndex])
	{
		float w = 0, x = 0, y = 0, z = 0, px = 0, py = 0, pz = 0, vfov = 90, hfov = 90;
		int32  viewnumber = 0;
		int32 ret = PicoImport::PvrGetMainSensorState(x, y, z, w, px, py, pz, hfov,vfov,viewnumber);
		if (ret == 0)
		{
			GCurrentOrientation = FQuat(-z, x, y, -w);
			if (GetDefault <UPicoMobileSettings>()->disableHmd6DofModule)
			{
				if (EnableNeckModel)
				{
					GCurrentPosition = GCurrentOrientation * NeckOffset - NeckOffset.Z * FVector::UpVector;
				}
				else
				{
					GCurrentPosition = FVector(0, 0, 0);
				}
			}
			else
			{
				GCurrentPosition = FVector(-pz * 100, px * 100, py * 100);
			}
			GFov = vfov;
			GVFov = vfov;
			GHFov = hfov;
			if (isInRenderThread)
				GViewnumber = viewnumber;

			enable = true;
		}
		else if (ret == -1)
			enable = false;

	}
	return enable;
}

bool PicoSDK::Sensor::GetPSensorState(int32& status)
{
	bool retB = false;

	status = PicoImport::PvrGetPsensorState();

	status >= 0 ? retB = true : retB = false;

	return retB;
}

bool PicoSDK::Sensor::Enable6Dof(bool enable)
{
	int32 _isSupport6Dof = -1;
	PicoImport::PvrGetIntConfig(PicoSDKConfig::GlobalIntConfigs::ABILITY6DOF, _isSupport6Dof);

	if (GIs6DofEnabled != enable&& _isSupport6Dof && !PicoImport::PvrEnable6DofModule(enable))
	{
		GIs6DofEnabled = enable;
		EnableNeckModel = !enable;
	}
	return GIs6DofEnabled;
}

bool PicoSDK::Sensor::GetAcceleration()
{
	bool enable = false;
	return enable;
}

bool PicoSDK::Sensor::GetAngularVelocity()
{
	bool enable = false;
	return enable;
}

//Neck Model
void PicoSDK::Sensor::GetNeckOffset()
{
	EnableNeckModel = GetDefault <UPicoMobileSettings>()->enableNeckModel;
	if (!GetDefault <UPicoMobileSettings>()->useCustomNeckParameter)
	{
		float neckx = 0.0f;
		float necky = 0.0f;
		float neckz = 0.0f;
		int32 modelx = (int32)PicoSDKConfig::GlobalFloatConfigs::NECK_MODEL_X;
		int32 modely = (int32)PicoSDKConfig::GlobalFloatConfigs::NECK_MODEL_Y;
		int32 modelz = (int32)PicoSDKConfig::GlobalFloatConfigs::NECK_MODEL_Z;
		PicoImport::PvrGetFloatConfig(modelx, neckx);
		PicoImport::PvrGetFloatConfig(modely, necky);
		PicoImport::PvrGetFloatConfig(modelz, neckz);
		if (neckx != 0.0f || necky != 0.0f || neckz != 0.0f)
		{
			NeckOffset = FVector(neckz, neckx, necky);
		}
	}
	else
	{
		NeckOffset = FVector(GetDefault <UPicoMobileSettings>()->neckOffset.Z, GetDefault <UPicoMobileSettings>()->neckOffset.X, GetDefault <UPicoMobileSettings>()->neckOffset.Y);
	}
}




const uint32 IS_DATA_FLAG = 0x80000000;
const uint32 DATA_POS_MASK = 0x40000000;
const int32 DATA_POS_SHIFT = 30;
const uint32 EVENT_TYPE_MASK = 0x3EFF0000;
const int32 EVENT_TYPE_SHIFT = 17;
const uint32 PAYLOAD_MASK = 0x0000FFFF;
const int32 PAYLOAD_SHIFT = 16;

static int32 EncodeType(int32 eventType)
{
	return (int32)((uint32)eventType & ~IS_DATA_FLAG); // make sure upper bit is not set
}

static int32 EncodeData(int32 eventId, int32 eventData, int32 pos)
{
	uint32 data = 0;
	data |= IS_DATA_FLAG;
	data |= (((uint32)pos << DATA_POS_SHIFT) & DATA_POS_MASK);
	data |= (((uint32)eventId << EVENT_TYPE_SHIFT) & EVENT_TYPE_MASK);
	data |= (((uint32)eventData >> (pos * PAYLOAD_SHIFT)) & PAYLOAD_MASK);

	return (int32)data;
}

FIntPoint PicoSDK::Render::GTextureSize = FIntPoint::ZeroValue;
int32 PicoSDK::Render::FFRLevel = 0;
float PicoSDK::Render::DisplayRate = 60;

void PicoSDK::Render::GetFOV(float& Fov)
{
	float _fov = 0;
#if PLATFORM_ANDROID
	PicoImport::PvrGetFloatConfig(PicoSDKConfig::GlobalFloatConfigs::VFOV, _fov);
	check(_fov > 0);
#endif
	Fov = _fov;
}
void PicoSDK::Render::GetFOV(float& VFov,float& HFov)
{
	float _vfov = 0,_hfov = 0;
#if PLATFORM_ANDROID
	PicoImport::PvrGetFloatConfig(PicoSDKConfig::GlobalFloatConfigs::VFOV, _vfov);	
	check(_vfov > 0);
	PicoImport::PvrGetFloatConfig(PicoSDKConfig::GlobalFloatConfigs::HFOV, _hfov);
	check(_hfov > 0);
#endif
	VFov = _vfov;
	HFov = _hfov;
}
void PicoSDK::Render::GetDisplayRefreshRate(float& RefreshRate)
{
	float _rate = 60.0f;
#if PLATFORM_ANDROID
	PicoImport::PvrGetFloatConfig(PicoSDKConfig::GlobalFloatConfigs::DISPLAY_REFRESH_RATE, _rate);
	check(_rate > 0);
	
#endif
	RefreshRate = _rate;
}

void PicoSDK::Render::GetIPD(float& Ipd)
{
	float _ipd = 0;
#if PLATFORM_ANDROID
	PicoImport::PvrGetFloatConfig(PicoSDKConfig::GlobalFloatConfigs::IPD, _ipd);
	check(_ipd > 0);
#endif
	Ipd = _ipd;
}

//FFR

bool PicoSDK::Render::isUseCustomFFRLevel = false;
FVector2D  PicoSDK::Render::CustomGainValue = FVector2D::ZeroVector;
float  PicoSDK::Render::CustomAreaValue = 0.f;
float  PicoSDK::Render::CustomMinimumValue = 0.f;

void PicoSDK::Render::SetFFRParameters(int textureId, int previousId,FVector EyeDirection)
{
	FVector2D FoveationGainValue = FVector2D::ZeroVector;
	float FoveationAreaValue = 0.f;
	float FoveationMinimumValue = 0.f;
	if (PicoSDK::Render::isUseCustomFFRLevel)
	{
		FoveationGainValue = PicoSDK::Render::CustomGainValue;
		FoveationAreaValue = PicoSDK::Render::CustomAreaValue;
		FoveationMinimumValue = PicoSDK::Render::CustomMinimumValue;
	}
	else
	{
		if (PicoSDK::Render::FFRLevel == 1)
		{
			FoveationGainValue = FVector2D(2.0f,2.0f);
			FoveationAreaValue = 0.0f;
			FoveationMinimumValue = 0.125f;
		}
		if (PicoSDK::Render::FFRLevel == 2)
		{
			FoveationGainValue = FVector2D(3.0f, 3.0f);
			FoveationAreaValue = 1.0f;
			FoveationMinimumValue = 0.125f;
		}
		if (PicoSDK::Render::FFRLevel == 3)
		{
			FoveationGainValue = FVector2D(4.0f, 4.0f);
			FoveationAreaValue = 2.0f;
			FoveationMinimumValue = 0.125f;
		}

	}
		PicoImport::PvrSetFoveationParameters(textureId, previousId, EyeDirection.X, EyeDirection.Y
		, FoveationGainValue.X, FoveationGainValue.Y
		, FoveationAreaValue,FoveationMinimumValue);
}

void PicoSDK::Render::SendFFREvent()
{
	PicoImport::PvrUnityRenderEvent(EncodeType(EVENT_BEGINEYE));
}

//MultiView
void PicoSDK::Render::EnableMultiView(bool isEnable)
{
	PicoImport::PvrEnableMultiView(isEnable);
}

void PicoSDK::Render::RenderThreadInit()
{
	PicoImport::PvrUnityRenderEvent(EncodeType(EVENT_INIT_RENDERTHREAD));
}

void PicoSDK::Render::PvrSetUnrealParam(int32 UnrealParamKey, int32 value)
{
	PicoImport::PvrSetUnrealParam(UnrealParamKey, value);
}

void PicoSDK::Render::RenderThreadShutdown()
{
	PicoImport::PvrUnityRenderEvent(EncodeType(EVENT_SHUTDOWN_RENDERTHREAD));
}

void PicoSDK::Render::RenderEventWithData(RenderEventType eventType, int32 eventData)
{
	PicoImport::PvrUnityRenderEvent(EncodeData((int32)eventType, eventData, 0));
	PicoImport::PvrUnityRenderEvent(EncodeData((int32)eventType, eventData, 1));
	PicoImport::PvrUnityRenderEvent(EncodeType((int32)eventType));
}
// Set CPU GPU level
void PicoSDK::Render::SetCPUGPULevels(int32 CPULevel, int32 GPULevel)
{  
#if PLATFORM_ANDROID
	PicoImport::PvrSetCPUGPULevels(CPULevel, GPULevel);
#endif
}
void PicoSDK::SendResumeEvent()
{
	PicoImport::PvrUnityRenderEvent(EncodeType(EVENT_RESUME));
}

void PicoSDK::SendPauseEvent()
{
	PicoImport::PvrUnityRenderEvent(EncodeType(EVENT_PAUSE));
}
// SeeThrough boundary

int32 PicoSDK::SeeThrough::SeeThroughState = 0;
int32 PicoSDK::SeeThrough::GetSeeThroughState()
{
	uint32 data = 0;
#if PLATFORM_ANDROID
	data = PicoImport::PvrGetSeeThroughState();
#endif
	return data;
}
bool PicoSDK::SeeThrough::GetBoundaryConfigured()
{
	bool ret = false;
#if PLATFORM_ANDROID
	ret = PicoImport::PvrGetBoundaryConfigured();
#endif					  	
	return ret;
}


bool PicoSDK::SeeThrough::GetBoundaryEnabled()
{
	bool ret = false;
#if PLATFORM_ANDROID
	ret = PicoImport::PvrGetBoundaryEnabled();
#endif
	return ret;
}

void PicoSDK::SeeThrough::SetBoundaryVisible(bool enable)
{
#if PLATFORM_ANDROID
	PicoImport::PvrSetBoundaryVisible(enable);
#endif
}

void PicoSDK::SeeThrough::SetReinPosition()
{
#if PLATFORM_ANDROID
	if (FPicoJava::GetLeftConConnectState())
	{
		FQuat LOri = FQuat::Identity;
		FVector LPos = FVector::ZeroVector;
		FPicoJava::GetLeftConState(LOri.X, LOri.Y, LOri.Z, LOri.W, LPos.X, LPos.Y, LPos.Z);
		int32 LTri = FPicoJava::GetLeftConTrigger();
		PicoImport::PvrSetReinPosition(LOri.X, LOri.Y, LOri.Z, LOri.W, LPos.X, LPos.Y, LPos.Z,0,true,LTri);
	}
	else
	{
		PicoImport::PvrSetReinPosition(0, 0, 0, 0, 0, 0, 0, 0, false, 0);
	}

	if (FPicoJava::GetRightConConnectState())
	{
		FQuat ROri = FQuat::Identity;
		FVector RPos = FVector::ZeroVector;
		FPicoJava::GetRightConState(ROri.X, ROri.Y, ROri.Z, ROri.W, RPos.X, RPos.Y, RPos.Z);
		int32 RTri = FPicoJava::GetRightConTrigger();
		PicoImport::PvrSetReinPosition(ROri.X, ROri.Y, ROri.Z, ROri.W, RPos.X, RPos.Y, RPos.Z, 1, true, RTri);
	}
	else
	{
		PicoImport::PvrSetReinPosition(0, 0, 0, 0, 0, 0, 0, 1, false, 0);
	}
#endif
}

#endif	