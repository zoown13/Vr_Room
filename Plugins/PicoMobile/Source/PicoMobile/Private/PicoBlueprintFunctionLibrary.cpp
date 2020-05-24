// Copyright Pico Technology Co, Ltd. All Rights Reserved.

#include "PicoBlueprintFunctionLibrary.h"	
#include "PicoMobileSettings.h"		   
#include "PicoImport.h"
#include "PicoJava.h"
#include "Misc/Paths.h"
#include "Misc/FileHelper.h"
#include "Engine/Texture2D.h"
#include "IImageWrapperModule.h"
#include "Modules/ModuleManager.h"
#include "IImageWrapper.h"

#include "../Launch/Resources/Version.h"

#if PLATFORM_ANDROID

#include "Android/AndroidApplication.h"
#include "Android/AndroidJNI.h"
#endif
#include "PicoMobile.h"
#include "PicoEyeTracker.h"

#define DEVICE_CV 1
#define DEVICE_GOBLIN 0

// Instantiate delegate
FPicoLogInOutCallbackDelegate UPicoBlueprintFunctionLibrary::PicoLogInOutCallback;
FPicoGetUserInfoCallbackDelegate UPicoBlueprintFunctionLibrary::PicoGetUserInfoCallback;
FPicoPayOrderCallbackDelegate UPicoBlueprintFunctionLibrary::PicoPayOrderCallback;
FPicoQueryOrderCallbackDelegate UPicoBlueprintFunctionLibrary::PicoQueryOrderCallback;
FPicoPaymentExceptionCallbackDelegate UPicoBlueprintFunctionLibrary::PicoPaymentExceptionCallback;
FPicoEntitlementVerifyCallbackDelegate UPicoBlueprintFunctionLibrary::PicoEntitlementVerifyCallback;

static FPicoMobileHMD* GetPicoHMD()
{
	if (GEngine->XRSystem.IsValid() && GEngine->XRSystem->GetSystemName() == FName("FPicoVRHMD"))
	{

		return static_cast<FPicoMobileHMD*>(GEngine->XRSystem.Get());
	}
	UE_LOG(LogTemp, Error, TEXT("UPicoBlueprintFunctionLibrary GetPicoHMD null"));
	return nullptr;
}

FQuat UPicoBlueprintFunctionLibrary::PicoGetCurrentOrientation()
{
#if PLATFORM_ANDROID
	return PicoSDK::Sensor::GCurrentOrientation;
#else
	return FQuat(0, 0, 0, 0);
#endif
}

FVector UPicoBlueprintFunctionLibrary::PicoGetCurrentPosition()
{
#if PLATFORM_ANDROID
	return PicoSDK::Sensor::GCurrentPosition;
#else
	return FVector(0, 0, 0);
#endif
}



void UPicoBlueprintFunctionLibrary::PicoIs6Dof(bool& is6dof)
{
#if PLATFORM_ANDROID
	if (GetDefault <UPicoMobileSettings>()->disableHmd6DofModule)
	{
		is6dof = false;
		return;
	}
	else
	{
		int32 whether6dof = -1;
		PicoImport::PvrGetIntConfig(PicoSDKConfig::GlobalIntConfigs::ABILITY6DOF, whether6dof);
		if (whether6dof == 1)
		{
			is6dof = true;
			return;
		}
		else
		{
			is6dof = false;
			return;
		}
	}
#endif
	return;
}

void UPicoBlueprintFunctionLibrary::PicoGetControllerDeviceType(FPicoControllerType& OutDeviceType)
{
	FPicoControllerType ControllerType = FPicoControllerType::Unknown;
#if PLATFORM_ANDROID
	int32 DeviceType = -1;
	int32 NeoDeviceTyp = -1;
	JNIEnv* Env = FAndroidApplication::GetJavaEnv();
	jmethodID GetDeviceType = FJavaWrapper::FindMethod(Env, FJavaWrapper::GameActivityClassID, "GetDeviceType", "()I", false);
	jmethodID GetNeoDeviceType = FJavaWrapper::FindMethod(Env, FJavaWrapper::GameActivityClassID, "GetNeoDeviceType", "()I", false);
	DeviceType = StaticCast<int32>(FJavaWrapper::CallIntMethod(Env, FJavaWrapper::GameActivityThis, GetDeviceType));
	NeoDeviceTyp = StaticCast<int32>(FJavaWrapper::CallIntMethod(Env, FJavaWrapper::GameActivityThis, GetNeoDeviceType));
	if (NeoDeviceTyp == 0)
	{
		jmethodID GetMainConnectState = FJavaWrapper::FindMethod(Env, FJavaWrapper::GameActivityClassID, "GetMainControllerConnectState", "()I", false);
		int32 MainControllerState = StaticCast<int32>(FJavaWrapper::CallIntMethod(Env, FJavaWrapper::GameActivityThis, GetMainConnectState));
		if (MainControllerState)
		{
			ControllerType = FPicoControllerType::PicoNeoController;
		}
		else
		{
			ControllerType = FPicoControllerType::PicoDisConnectController;
		}
	}
	else if (NeoDeviceTyp == 4)
	{
		jmethodID GetMainConnectState = FJavaWrapper::FindMethod(Env, FJavaWrapper::GameActivityClassID, "GetMainControllerConnectState", "()I", false);
		int32 MainControllerState = StaticCast<int32>(FJavaWrapper::CallIntMethod(Env, FJavaWrapper::GameActivityThis, GetMainConnectState));
		if (MainControllerState)
		{
			ControllerType = FPicoControllerType::PicoNeo2Controller;
		}
		else
		{
			ControllerType = FPicoControllerType::PicoDisConnectController;
		}
	}
	else
	{
		if (DeviceType == 1)
		{
			ControllerType = FPicoControllerType::PicoGoblinController;
		}
		else if (DeviceType == 3)
		{
			ControllerType = FPicoControllerType::PicoGublin2Controller;
		}
		else
		{
			ControllerType = FPicoControllerType::PicoDisConnectController;
		}
	}
#endif	
	OutDeviceType = ControllerType;

}

//
//void UPicoBlueprintFunctionLibrary::PicoInitPSensor()
//{
//#if PLATFORM_ANDROID
//	if (JNIEnv* Env = FAndroidApplication::GetJavaEnv())
//	{
//		static jmethodID Method = FJavaWrapper::FindMethod(Env, FJavaWrapper::GameActivityClassID, "InitPSensor", "()V", false);
//		FJavaWrapper::CallVoidMethod(Env, FJavaWrapper::GameActivityThis, Method);
//	}
//#endif
//}

void UPicoBlueprintFunctionLibrary::PicoGetPSensorState(int32& SensorState)
{
	int32 state = -1;
	if (GetDefault <UPicoMobileSettings>()->EnablePSensor) 
	{
#if PLATFORM_ANDROID
		if (JNIEnv* Env = FAndroidApplication::GetJavaEnv())
		{
			static jmethodID Method = FJavaWrapper::FindMethod(Env, FJavaWrapper::GameActivityClassID, "AndroidThunkJava_UEPvr_GetPsensorState", "()I", false);
			state = FJavaWrapper::CallIntMethod(Env, FJavaWrapper::GameActivityThis, Method);	
		}
#endif
	}
	if (state == 0)
	{
		SensorState = 0;
	} 
	else if (state == -1)
	{
		SensorState = -1;
	}
	else
	{
		SensorState = 1;
	}

}

//void UPicoBlueprintFunctionLibrary::PicoUnRegisterPSensor()
//{
//#if PLATFORM_ANDROID
//	if (JNIEnv* Env = FAndroidApplication::GetJavaEnv())
//	{
//		static jmethodID Method = FJavaWrapper::FindMethod(Env, FJavaWrapper::GameActivityClassID, "UnRegisterPSensor", "()V", false);
//		FJavaWrapper::CallVoidMethod(Env, FJavaWrapper::GameActivityThis, Method);
//	}
//#endif
//
//}

bool UPicoBlueprintFunctionLibrary::ResetSensor()
{
#if PLATFORM_ANDROID
	return PicoSDK::Sensor::ResetSensor(DEVICE_GOBLIN);
#else
	return false;
#endif
}
float UPicoBlueprintFunctionLibrary::PicoGetFov()
{
#if PLATFORM_ANDROID
	return PicoSDK::Sensor::GFov;
#else
	return 0.0f;
#endif
}
float UPicoBlueprintFunctionLibrary::PicoGetVFov()
{
#if PLATFORM_ANDROID
	return PicoSDK::Sensor::GVFov;
#else
	return 0.0f;
#endif
}
float UPicoBlueprintFunctionLibrary::PicoGetHFov()
{
#if PLATFORM_ANDROID
	return PicoSDK::Sensor::GHFov;
#else
	return 0.0f;
#endif
}

int32 UPicoBlueprintFunctionLibrary::PicoGetCurrentBrightness()
{
#if PLATFORM_ANDROID
	return FPicoJava::GetCurrentBrightness();
#else
	return 0;
#endif
}

bool UPicoBlueprintFunctionLibrary::PicoSetBrightness(int32 Brightness)
{
#if PLATFORM_ANDROID
	return FPicoJava::SetBrightness(Brightness);
#else
	return false;
#endif
}

bool UPicoBlueprintFunctionLibrary::PicoGetGpuUtilization(float &GpuUtilization)
{
	bool Succee = false;
	float Utilization = -1;
#if PLATFORM_ANDROID
	if (PicoImport::PvrGetGpuUtilization != nullptr)
	{
		Utilization = PicoImport::PvrGetGpuUtilization();
		Succee = true;
	}
#endif
	GpuUtilization = Utilization;
	return Succee;
}

int32 UPicoBlueprintFunctionLibrary::PicoGetCurrentVolume()
{
#if PLATFORM_ANDROID
	return FPicoJava::GetCurrentVolume();
#else
	return 0;
#endif
}

int32 UPicoBlueprintFunctionLibrary::PicoGetMaxVolumeNumber()
{
#if PLATFORM_ANDROID
	return FPicoJava::GetMaxVolumeNumber();
#else
	return 0;
#endif
}

void UPicoBlueprintFunctionLibrary::PicoSetVolume(int32 Volume)
{
#if PLATFORM_ANDROID
	return FPicoJava::SetVolume(Volume);
#endif
}

void UPicoBlueprintFunctionLibrary::PicoVolumeUp()
{
#if PLATFORM_ANDROID
	return FPicoJava::VolumeUp();
#endif
}

void UPicoBlueprintFunctionLibrary::PicoVolumeDown()
{
#if PLATFORM_ANDROID
	return FPicoJava::VolumeDown();
#endif
}

int32 UPicoBlueprintFunctionLibrary::PicoGetHandness()
{
#if PLATFORM_ANDROID
	return FPicoJava::GetHandness();
#else
	return 2;
#endif
}

void UPicoBlueprintFunctionLibrary::PicoGetDeviceSN(FString & SN)
{
#if PLATFORM_ANDROID
	SN = FPicoJava::GetDeviceSN();
#endif
}

void UPicoBlueprintFunctionLibrary::SetCPUAndGPULevels(int CPULevel,int GPULevel)
{
#if PLATFORM_ANDROID
	PicoImport::PvrSetCPUGPULevels(CPULevel, GPULevel);
#endif
}


bool UPicoBlueprintFunctionLibrary::PicoControllerHomekey()
{
	bool enable = false;
#if PLATFORM_ANDROID
	enable = GetDefault <UPicoMobileSettings>()->enableHomeKey;
#endif
		return enable;
}


void UPicoBlueprintFunctionLibrary::PicoGetDeviceModel(FString & Model)
{		
#if PLATFORM_ANDROID
	Model = FAndroidMisc::GetDeviceModel();
#endif
}

EPicoSimulationType UPicoBlueprintFunctionLibrary::PicoIsCurrentDeviceValid()
{
#if PLATFORM_ANDROID
	if (GetDefault <UPicoMobileSettings>()->EntitlementCheckSimulation)
	{
		int32 num = GetDefault <UPicoMobileSettings>()->DeviceSN.Num();
		if (num == 0)
		{
			return EPicoSimulationType::Null;
		}
		else
		{
			for (int i = 0; i < num; i++)
			{
				if (GetDefault <UPicoMobileSettings>()->DeviceSN[i] == FPicoJava::GetDeviceSN())
				{
					return EPicoSimulationType::Valid;
				}
			}
			return EPicoSimulationType::Invalid;
		}
	}
	else
	{
		return EPicoSimulationType::Invalid;
	}
#endif
	return EPicoSimulationType::Invalid;
}

bool UPicoBlueprintFunctionLibrary::PicoAppEntitlementCheck(FString appID)
{
#if PLATFORM_ANDROID
	FString publicKey = "";
	return FPicoJava::VerifyAPP(appID, publicKey);
#endif
	return false;
}

bool UPicoBlueprintFunctionLibrary::PicoKeyEntitlementCheck(FString publicKey)
{
#if PLATFORM_ANDROID
	FString appID = "";
	return FPicoJava::VerifyAPP(appID, publicKey);
#endif
	return false;
}

void UPicoBlueprintFunctionLibrary::PicoEntitlementVerifySetCallbackDelegates(
	FPicoEntitlementVerifyCallbackDelegate OnPicoEntitlementVerifyCallback)
{
#if PLATFORM_ANDROID
	PicoEntitlementVerifyCallback = OnPicoEntitlementVerifyCallback;
#endif
}

void UPicoBlueprintFunctionLibrary::PicoLoginSDK()
{
#if PLATFORM_ANDROID
	FPicoJava::PaymentLogin();
#endif
}

void UPicoBlueprintFunctionLibrary::PicoPaymentLogin()
{
#if PLATFORM_ANDROID
	FPicoJava::PaymentLogin();
#endif
}

void UPicoBlueprintFunctionLibrary::PicoLogoutSDK()
{
#if PLATFORM_ANDROID
	FPicoJava::PaymentLogout();
#endif
}

void UPicoBlueprintFunctionLibrary::PicoPaymentLogout()
{
#if PLATFORM_ANDROID
	FPicoJava::PaymentLogout();
#endif
}

void UPicoBlueprintFunctionLibrary::PicoSDKGetUserInfo()
{
#if PLATFORM_ANDROID
	FPicoJava::PaymentGetUserInfo();
#endif
}

void UPicoBlueprintFunctionLibrary::PicoPaymentGetUserInfo()
{
#if PLATFORM_ANDROID
	FPicoJava::PaymentGetUserInfo();
#endif
}

void UPicoBlueprintFunctionLibrary::PicoPaymentPayWithCoin(const FPicoCoinOrderInfo& Order)
{
#if PLATFORM_ANDROID
	FPicoJava::PaymentPayWithCoin(Order.OrderNumber, Order.OrderTitle, Order.ProductDetail, Order.NotifyUrl, Order.PicoCoinCount);
#endif
}

void UPicoBlueprintFunctionLibrary::PicoPaymentPayWithPayCode(const FPicoPayCodeOrderInfo& Order)
{
#if PLATFORM_ANDROID
	FPicoJava::PaymentPayWithPayCode(Order.OrderNumber, Order.OrderTitle, Order.ProductDetail, Order.NotifyUrl, Order.PicoPayCode);
#endif
}

void UPicoBlueprintFunctionLibrary::PicoPaymentQueryOrder(const FString& OrderNumber)
{
#if PLATFORM_ANDROID
	FPicoJava::PaymentQueryOrder(OrderNumber);
#endif
}
extern FString GExternalFilePath;
FString UPicoBlueprintFunctionLibrary::GetRootPath() 
{
	FString PathStr = "";
#if PLATFORM_ANDROID
	TArray<FString> Folders;
	GExternalFilePath.ParseIntoArray(Folders, TEXT("/"));
	for (FString Folder : Folders)
	{
		PathStr += FString("/..");
	}

#endif
	//UE_LOG(LogTemp, Warning, TEXT("Pvr_UE LoadMesh RootPathe %s "), *PathStr);
	return PathStr;
}

bool UPicoBlueprintFunctionLibrary::PicoGetFFRLevel(PicoFFRLevel& FFRLevel)
{
	bool ret = false;
	FFRLevel = PicoFFRLevel::Off;
#if PLATFORM_ANDROID
	if (PicoSDK::Render::isUseCustomFFRLevel)
	{
		return false;
	}
	switch (PicoSDK::Render::FFRLevel)
	{
	case 0:
		FFRLevel = PicoFFRLevel::Off;
		break;
	case 1:
		FFRLevel = PicoFFRLevel::Low;
		break; 
	case 2:
		FFRLevel = PicoFFRLevel::Medium;
		break;
	case 3:
		FFRLevel = PicoFFRLevel::High;
		break;
	default:
		break;
	}
	ret = true;
#endif
	return ret;
	
}

bool UPicoBlueprintFunctionLibrary::PicoSetFFRLevel(PicoFFRLevel FFRLevel)
{
	bool ret = false;
#if PLATFORM_ANDROID
	PicoSDK::Render::isUseCustomFFRLevel = false;
	switch (FFRLevel)
	{
	case PicoFFRLevel::Off:
		PicoSDK::Render::FFRLevel = 0;
		break;
	case PicoFFRLevel::Low:
		PicoSDK::Render::FFRLevel = 1;
		break;
	case PicoFFRLevel::Medium:
		PicoSDK::Render::FFRLevel = 2;
		break;
	case PicoFFRLevel::High:
		PicoSDK::Render::FFRLevel = 3;
		break;
	default:
		break;
	}
	ret = true;
#endif
	return ret;
}

bool UPicoBlueprintFunctionLibrary::PicoGetFFRParameter(FVector2D &FoveationGainValue, float &FoveationAreaValue, float &FoveationMinimumValue)
{
#if PLATFORM_ANDROID
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
			FoveationGainValue = FVector2D(2.0f, 2.0f);
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
		if (PicoSDK::Render::FFRLevel == 0)
		{
			FoveationGainValue = FVector2D::ZeroVector;
			FoveationAreaValue = 0;
			FoveationMinimumValue = 0;
		}
	}
	return true;
#endif
	return false;
}

bool UPicoBlueprintFunctionLibrary::PicoSetFFRParameter(FVector2D FoveationGainValue, float FoveationAreaValue, float FoveationMinimumValue)
{
#if PLATFORM_ANDROID
	PicoSDK::Render::isUseCustomFFRLevel = true;
	PicoSDK::Render::CustomGainValue = FoveationGainValue;
	PicoSDK::Render::CustomAreaValue = FoveationAreaValue;
	PicoSDK::Render::CustomMinimumValue = FoveationMinimumValue;
	return true;
#endif
	return false;
}

bool UPicoBlueprintFunctionLibrary::PicoGetCurrentDisplayFrequency(float &CurrentDisplayFrequency)
{
	bool ret = false;	
	CurrentDisplayFrequency = 60.0f;
#if PLATFORM_ANDROID
	CurrentDisplayFrequency = PicoSDK::Render::DisplayRate;	
#endif
	ret  = true;
	return ret;
}
//boundary
bool UPicoBlueprintFunctionLibrary::PicoGetBoundaryConfigured()
{
	bool ret = false;
#if PLATFORM_ANDROID
	ret = PicoSDK::SeeThrough::GetBoundaryConfigured();
#endif
	return ret;
}

bool UPicoBlueprintFunctionLibrary::PicoGetBoundaryEnabled()
{
	bool ret = false;
#if PLATFORM_ANDROID
	ret = PicoSDK::SeeThrough::GetBoundaryEnabled();
#endif
	return ret;
}

void UPicoBlueprintFunctionLibrary::PicoSetBoundaryVisible(bool enable)
{
	
#if PLATFORM_ANDROID
	PicoSDK::SeeThrough::SetBoundaryVisible(enable);
#endif
}


#pragma region Eyetracking

bool UPicoBlueprintFunctionLibrary::PicoGetEyeTrackingGazeRay(FVector& Origin, FVector& Direction)
{
	FEyeTrackingGazeRay outEyeTrackingGazeRay;
	memset(&outEyeTrackingGazeRay, 0, sizeof(outEyeTrackingGazeRay));
#if PLATFORM_ANDROID
	if ((PicoImport::PvrGetTrackingMode() & 4) && GetDefault<UPicoMobileSettings>()->EnableEyeTracking)
	{
		FPicoEyeTracker *EyeTracker = FPicoEyeTracker::GetInstance();
		if (EyeTracker->GetEyeTrackingGazeRay(outEyeTrackingGazeRay))
		{
			Direction = outEyeTrackingGazeRay.Direction;
			Origin = outEyeTrackingGazeRay.Origin;
			return true;
		}
	}
#endif
	return false;
}

/*bool UPicoBlueprintFunctionLibrary::PicoGetEyeDirection(FVector &OutEyeDirection)
{
	bool Success = false;
	FPicoMobileHMD* PicoHMD = GetPicoHMD();
	if (nullptr == PicoHMD ||false == GetDefault<UPicoMobileSettings>()->EnableEyeTracking)
	{
		return Success;
	}

	Success = PicoHMD->GetRelativeEyeDirection(PicoHMD->HMDDeviceId , OutEyeDirection);
	
	//UE_LOG(LogTemp, Log, TEXT("OutEyeDirection1  (%f , %f , %f) "), OutEyeDirection.X, OutEyeDirection.Y, OutEyeDirection.Z);

	FQuat HMD_Orientation;
	FVector HMD_Position;

	// rotate eye gaze by HMD rotation
	bool ret = PicoHMD->GetCurrentPose(PicoHMD->HMDDeviceId, HMD_Orientation, HMD_Position);
	if (Success && ret)
	{
		OutEyeDirection = HMD_Orientation * OutEyeDirection;
		//UE_LOG(LogTemp, Log, TEXT("OutEyeDirection2  (%f , %f , %f) "), OutEyeDirection.X, OutEyeDirection.Y, OutEyeDirection.Z);
		return true;
	}
	return false;
}

void UPicoBlueprintFunctionLibrary::PicoGetEyeOrientationAndPosition(FVector& OutPosition, FQuat& OutOrientation, FRotator& OutRotation, bool& Success)
{
	Success = false;
#if PLATFORM_ANDROID
	FPicoMobileHMD* PicoHMD = GetPicoHMD();

	if (nullptr == PicoHMD || false == PicoHMD->isEyeTrackingEnabled())
	{
		return;
	}

	// doesn't care about eye parameter....
	Success = PicoHMD->GetRelativeEyePose(PicoHMD->HMDDeviceId, EStereoscopicPass::eSSP_LEFT_EYE, OutOrientation, OutPosition);

	//UE_LOG(LogTemp, Log, TEXT("OutOrientation1  (%f , %f , %f , %f) "), OutOrientation.X, OutOrientation.Y, OutOrientation.Z, OutOrientation.W);
	//UE_LOG(LogTemp, Log, TEXT("OutPosition1  (%f , %f , %f) "), OutPosition.X, OutPosition.Y, OutPosition.Z);
	FQuat HMD_Orientation;
	FVector HMD_Position;

	if (Success && (Success = PicoHMD->GetCurrentPose(PicoHMD->HMDDeviceId, HMD_Orientation, HMD_Position)))
	{
		OutPosition = HMD_Orientation * OutPosition + HMD_Position;
		OutOrientation = HMD_Orientation * OutOrientation;
		OutRotation = OutOrientation.Rotator();
		//UE_LOG(LogTemp, Log, TEXT("OutRotation2  (%f , %f , %f) "), OutRotation.Roll, OutRotation.Pitch, OutRotation.Yaw);
		//UE_LOG(LogTemp, Log, TEXT("OutOrientation2  (%f , %f , %f , %f) "), OutOrientation.X, OutOrientation.Y, OutOrientation.Z, OutOrientation.W);
		//UE_LOG(LogTemp, Log, TEXT("OutPosition2  (%f , %f , %f) "), OutPosition.X, OutPosition.Y, OutPosition.Z);
}
	else
	{
		UE_LOG(LogTemp, Log, TEXT("PicoGetEyeOrientationAndPosition Failed "));
	}

#endif
	return;
}



void UPicoBlueprintFunctionLibrary::PicoGetEyeOpenness(EPicoVRHMD_Eye WhichEye, float& Openness, bool& Success)
{
#if PLATFORM_ANDROID
	FPicoMobileHMD* PicoHMD = GetPicoHMD();
	Success = false;

	if (nullptr == PicoHMD || false == PicoHMD->isEyeTrackingEnabled())
	{
		return;
	}
	FEyeTrackingData eps;
	if (PicoHMD->GetEyeTrackingDataFromDevice(eps))
	{
		// Return either eye or averaged value
		Openness = WhichEye == EPicoVRHMD_Eye::LeftEye ? eps.leftEyeOpenness :
			(WhichEye == EPicoVRHMD_Eye::RightEye ? eps.rightEyeOpenness : (eps.leftEyeOpenness + eps.rightEyeOpenness)*0.5f);

		Success = true;
	}
	else
	{
		UE_LOG(LogTemp, Log, TEXT("PicoGetEyeOpenness Failed!"));
	}

#endif
	return;

}

void UPicoBlueprintFunctionLibrary::PicoGetPupilDialation(EPicoVRHMD_Eye WhichEye, float& Dialation, bool& Success)
{
#if PLATFORM_ANDROID
	FPicoMobileHMD* PicoHMD = GetPicoHMD();
	Success = false;

	if (nullptr == PicoHMD || false == PicoHMD->isEyeTrackingEnabled())
	{
		return;
	}

	FEyeTrackingData eps;
	if (PicoHMD->GetEyeTrackingDataFromDevice(eps))
	{	
		// Return either eye or averaged value
		Dialation = WhichEye == EPicoVRHMD_Eye::LeftEye ? eps.leftEyePupilDilation :
			(WhichEye == EPicoVRHMD_Eye::RightEye ? eps.rightEyePupilDilation : (eps.leftEyePupilDilation + eps.rightEyePupilDilation)*0.5f);

		Success = true;
	}
	else
	{
		UE_LOG(LogTemp, Log, TEXT("PicoGetPupilDialation Failed! "));
	}
#endif
	return;
}

void UPicoBlueprintFunctionLibrary::PicoIsEyetrackingEnabled(bool& IsEnabled)
{
#if PLATFORM_ANDROID
	FPicoMobileHMD* PicoHMD = GetPicoHMD();
	IsEnabled = (nullptr != PicoHMD && PicoHMD->isEyeTrackingEnabled()) ? true : false;
#endif
}*/

#pragma endregion Eyetracking
void UPicoBlueprintFunctionLibrary::LoadMesh(FString FilePathAndName, TArray<FVector> &Vertices, TArray<int32> &Triangles, TArray<FVector> &Normals, TArray<FVector2D> &UV, FString &Name,bool &Succeed)
{
	FString LoadedString;
	TArray<FString> LineArray = {};
	TArray<FString> VArry = {};
	TArray<FString> VTArry = {};
	TArray<FString> VNArry = {};
	TArray<FString> GArry = {};
	TArray<FString> FTempArry = {};
	TArray<FVector> VTempArry = {};
	TArray<FVector2D> VTTempArry = {};
	TArray<FVector> VNTempArry = {};
	TArray<FString> FArry = {};
	TArray<FString> FDataArry = {};
	TArray<int32> IntArry = {};
	int32 faceDataCount = 0;
	TArray<int32>triangles = {};
	TArray<FVector>FaceData = {};

	IPlatformFile& PlatformFile = FPlatformFileManager::Get().GetPlatformFile();
	FilePathAndName = GetRootPath() + FilePathAndName;
	if (PlatformFile.FileExists(*FilePathAndName))
	{
		//UE_LOG(LogTemp, Warning, TEXT("Pvr_UE LoadMesh File %s Succeed"),*FilePathAndName);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Pvr_UE LoadMesh File %s Faild"), *FilePathAndName);
		Succeed = false;
		return;
	}
	FFileHelper::LoadFileToString(LoadedString, *FilePathAndName);
	LoadedString.ParseIntoArrayLines(LineArray, true);
	for (int i = 0; i < LineArray.Num(); i++)
	{
		if (LineArray[i].StartsWith("v "))
		{
			LineArray[i].ParseIntoArray(VArry, TEXT(" "), true);
			FVector Temp = FVector::ZeroVector;
			for (int index = 0; index < VArry.Num(); index++)
			{
				switch (index % 4)
				{
				case 0:
					break;
				case 1:
					Temp.X = FCString::Atof(*VArry[index]);
					break;
				case 2:
					Temp.Y = FCString::Atof(*VArry[index]);
					break;
				case 3:
					Temp.Z = -FCString::Atof(*VArry[index]);
					VTempArry.Add(Temp);
					break;
				}
			}
		}
		else if (LineArray[i].StartsWith("vt"))
		{
			FVector2D Temp = FVector2D::ZeroVector;
			LineArray[i].ParseIntoArray(VTArry, TEXT(" "), true);
			for (int index = 0; index < VTArry.Num(); index++)
			{
				switch (index % 3)
				{
				case 0:
					break;
				case 1:
					Temp.X = FCString::Atof(*VTArry[index]);
					break;
				case 2:
					Temp.Y = -FCString::Atof(*VTArry[index]);
					VTTempArry.Add(Temp);
					break;
				}
			}
		}
		else if (LineArray[i].StartsWith("vn"))
		{
			FVector Temp = FVector::ZeroVector;
			LineArray[i].ParseIntoArray(VNArry, TEXT(" "), true);
			for (int index = 0; index < VNArry.Num(); index++)
			{
				switch (index % 4)
				{
				case 0:
					break;
				case 1:
					Temp.X = FCString::Atof(*VNArry[index]);
					break;
				case 2:
					Temp.Y = FCString::Atof(*VNArry[index]);
					break;
				case 3:
					Temp.Z = -FCString::Atof(*VNArry[index]);
					VNTempArry.Add(Temp);
					break;
				}
			}
		}
		else if (LineArray[i].StartsWith("g "))//name
		{
			LineArray[i].ParseIntoArray(GArry, TEXT(" "), true);
		}

		else if (LineArray[i].StartsWith("f "))
		{
			int32 j = 1;
			int32 Info = 0;
			IntArry = {};
			LineArray[i].ParseIntoArray(FArry, TEXT(" "), true);
			for (int index = 0; index < FArry.Num(); index++)
			{
				if (index != 0)
				{
					j++;
					FTempArry.Add(FArry[index]);
					IntArry.Add(faceDataCount);
					faceDataCount++;
				}
			}
			Info += j;
			j = 1;
			while (j + 2 < Info)
			{
				triangles.Add(IntArry[0]);
				triangles.Add(IntArry[j]);
				triangles.Add(IntArry[j + 1]);
				j++;
			}
		}
	}

	for (int i = 0; i < FTempArry.Num(); i++)
	{
		FVector Temp = FVector::ZeroVector;
		FTempArry[i].ParseIntoArray(FDataArry, TEXT("/"), true);
		for (int index = 0; index < FDataArry.Num(); index++)
		{
			switch (index % 3)
			{
			case 0:
				Temp.X = FCString::Atof(*FDataArry[index]);
				break;
			case 1:
				Temp.Y = FCString::Atof(*FDataArry[index]);
				break;
			case 2:
				Temp.Z = FCString::Atof(*FDataArry[index]);
				//UE_LOG(LogTemp, Warning, TEXT("FVector %f,%f,%f"), Temp.X,Temp.Y,Temp.Z);
				FaceData.Add(Temp);
				break;
			}
		}
	}

	TArray<FVector> newVerts = {};
	TArray<FVector> newNormals = {};
	TArray<FVector2D> newUVs = {};

	for (int i = 0; i < FaceData.Num(); i++)
	{
		newVerts.Add(FVector::ZeroVector);
		newNormals.Add(FVector::ZeroVector);
		newUVs.Add(FVector2D::ZeroVector);
	}

	for (int i = 0; i < FaceData.Num(); i++)
	{
		newVerts[i] = VTempArry[FaceData[i].X - 1];
		if (FaceData[i].Y >= 1)
		{
			newUVs[i] = VTTempArry[FaceData[i].Y - 1];
		}
		if (FaceData[i].Z >= 1)
		{
			newNormals[i] = VNTempArry[FaceData[i].Z - 1];
		}
	}


	Vertices = newVerts;
	Normals = newNormals;
	UV = newUVs;
	Name = GArry[1];
	Triangles = triangles;
	Succeed = true;
}


void UPicoBlueprintFunctionLibrary::LoadTexture(FString FilePathAndName, UTexture2D* &Texture, bool &Succeed)
{
	UTexture2D* OutTex = NULL;
	IImageWrapperModule& imageWrapperModule = FModuleManager::LoadModuleChecked<IImageWrapperModule>(FName("ImageWrapper"));
	TSharedPtr<IImageWrapper> imageWrapper = imageWrapperModule.CreateImageWrapper(EImageFormat::PNG);

	TArray<uint8> OutArray;
	IPlatformFile& PlatformFile = FPlatformFileManager::Get().GetPlatformFile();
	FilePathAndName = GetRootPath() + FilePathAndName;
	if (PlatformFile.FileExists(*FilePathAndName))
	{
		//UE_LOG(LogTemp, Warning, TEXT("Pvr_UE LoadTexture File %s Succeed"), *FilePathAndName);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Pvr_UE LoadTexture File %s Faild"), *FilePathAndName);
		Succeed = false;
		return;
	}
	if (FFileHelper::LoadFileToArray(OutArray, *FilePathAndName))
	{
		if (imageWrapper.IsValid() &&
			imageWrapper->SetCompressed(OutArray.GetData(), OutArray.Num()))
		{
			const TArray<uint8>* uncompressedRGBA = NULL;
			if (imageWrapper->GetRaw(ERGBFormat::RGBA, 8, uncompressedRGBA))
			{
				const TArray<FColor> uncompressedFColor = uint8ToFColor(*uncompressedRGBA);
				OutTex = TextureFromImage(
					imageWrapper->GetWidth(),
					imageWrapper->GetHeight(),
					uncompressedFColor,
					true);
			}
		}
	}
	else
	{
		Succeed = false;
		return;
	}
	Succeed = true;
	Texture = OutTex;

}

TArray<FColor> UPicoBlueprintFunctionLibrary::uint8ToFColor(const TArray<uint8> origin)
{
	TArray<FColor> uncompressedFColor;
	uint8 auxOrigin;
	FColor auxDst;

	for (int i = 0; i < origin.Num(); i++) {
		auxOrigin = origin[i];
		auxDst.R = auxOrigin;
		i++;
		auxOrigin = origin[i];
		auxDst.G = auxOrigin;
		i++;
		auxOrigin = origin[i];
		auxDst.B = auxOrigin;
		i++;
		auxOrigin = origin[i];
		auxDst.A = auxOrigin;
		uncompressedFColor.Add(auxDst);
	}

	return  uncompressedFColor;
}

UTexture2D* UPicoBlueprintFunctionLibrary::TextureFromImage(const int32 SrcWidth, const int32 SrcHeight, const TArray<FColor> &SrcData, const bool UseAlpha)
{
	UTexture2D* MyScreenshot = UTexture2D::CreateTransient(SrcWidth, SrcHeight, PF_B8G8R8A8);
	uint8* MipData = static_cast<uint8*>(MyScreenshot->PlatformData->Mips[0].BulkData.Lock(LOCK_READ_WRITE));

	uint8* DestPtr = NULL;
	const FColor* SrcPtr = NULL;
	for (int32 y = 0; y < SrcHeight; y++)
	{
		DestPtr = &MipData[(SrcHeight - 1 - y) * SrcWidth * sizeof(FColor)];
		SrcPtr = const_cast<FColor*>(&SrcData[(SrcHeight - 1 - y) * SrcWidth]);
		for (int32 x = 0; x < SrcWidth; x++)
		{
			*DestPtr++ = SrcPtr->B;
			*DestPtr++ = SrcPtr->G;
			*DestPtr++ = SrcPtr->R;
			if (UseAlpha)
			{
				*DestPtr++ = SrcPtr->A;
			}
			else
			{
				*DestPtr++ = 0xFF;
			}
			SrcPtr++;
		}
	}
	MyScreenshot->PlatformData->Mips[0].BulkData.Unlock();
	MyScreenshot->UpdateResource();

	return MyScreenshot;
}

void UPicoBlueprintFunctionLibrary::PicoPaymentSetCallbackDelegates(
	FPicoLogInOutCallbackDelegate OnPicoLogInOutCallback,
	FPicoGetUserInfoCallbackDelegate OnPicoGetUserInfoCallback,
	FPicoPayOrderCallbackDelegate OnPicoPayOrderCallback,
	FPicoQueryOrderCallbackDelegate OnPicoQueryOrderCallback,
	FPicoPaymentExceptionCallbackDelegate OnPicoPaymentExceptionCallback
)
{
#if PLATFORM_ANDROID
	PicoLogInOutCallback = OnPicoLogInOutCallback;
	PicoGetUserInfoCallback = OnPicoGetUserInfoCallback;
	PicoPayOrderCallback = OnPicoPayOrderCallback;
	PicoQueryOrderCallback = OnPicoQueryOrderCallback;
	PicoPaymentExceptionCallback = OnPicoPaymentExceptionCallback;
	
#endif
}

#if PLATFORM_ANDROID
extern "C" JNIEXPORT void  JNICALL Java_com_epicgames_ue4_GameActivity_nativePicoLogInOutCallback(JNIEnv * env, jclass clazz, jboolean isSuccess, jstring reason)
{
	bool _isSuccess = (bool)isSuccess;
	FString _reason;
	if (reason != NULL)
	{
		const char* JavaChars = env->GetStringUTFChars(reason, 0);
		_reason = FString(UTF8_TO_TCHAR(JavaChars));
		env->ReleaseStringUTFChars(reason, JavaChars);
	}

	if (UPicoBlueprintFunctionLibrary::PicoLogInOutCallback.IsBound())
	{
		UPicoBlueprintFunctionLibrary::PicoLogInOutCallback.Execute(_isSuccess,_reason);
	}
}
extern "C" JNIEXPORT void  JNICALL Java_com_epicgames_ue4_GameActivity_nativePicoGetUserInfoCompleteCallback(JNIEnv * env, jclass clazz, jstring info)
{
	FString _info;
	if (info != NULL)
	{
		const char* JavaChars = env->GetStringUTFChars(info, 0);
		_info = FString(UTF8_TO_TCHAR(JavaChars));
		env->ReleaseStringUTFChars(info, JavaChars);
	}

	if (UPicoBlueprintFunctionLibrary::PicoGetUserInfoCallback.IsBound())
	{
		UPicoBlueprintFunctionLibrary::PicoGetUserInfoCallback.Execute(_info);
	}

}
extern "C" JNIEXPORT void  JNICALL Java_com_epicgames_ue4_GameActivity_nativePicoPaymentExceptionCallback(JNIEnv * env, jclass clazz, jstring exception)
{
	FString _exception;
	if (exception != NULL)
	{
		const char* JavaChars = env->GetStringUTFChars(exception, 0);
		_exception = FString(UTF8_TO_TCHAR(JavaChars));
		env->ReleaseStringUTFChars(exception, JavaChars);
	}

	if (UPicoBlueprintFunctionLibrary::PicoGetUserInfoCallback.IsBound())
	{
		UPicoBlueprintFunctionLibrary::PicoGetUserInfoCallback.Execute(_exception);
	}
}

extern "C" JNIEXPORT void  JNICALL Java_com_epicgames_ue4_GameActivity_nativePicoPayOrderCallback(JNIEnv * env, jclass clazz, jstring code, jstring msg)
{
	FString _code , _msg;
	if (code != NULL && msg != NULL)
	{
		const char* JavaChars = env->GetStringUTFChars(code, 0);
		_code = FString(UTF8_TO_TCHAR(JavaChars));
		env->ReleaseStringUTFChars(code, JavaChars);

		JavaChars = env->GetStringUTFChars(msg, 0);
		_msg = FString(UTF8_TO_TCHAR(JavaChars));
		env->ReleaseStringUTFChars(msg, JavaChars);
	}

	if (UPicoBlueprintFunctionLibrary::PicoPayOrderCallback.IsBound())
	{
		UPicoBlueprintFunctionLibrary::PicoPayOrderCallback.Execute(_code,_msg);
	}
}

extern "C" JNIEXPORT void  JNICALL Java_com_epicgames_ue4_GameActivity_nativePicoQueryOrdersCallback(JNIEnv * env, jclass clazz, jstring code, jstring msg)
{
	FString _code, _msg;
	if (code != NULL && msg != NULL)
	{
		const char* JavaChars = env->GetStringUTFChars(code, 0);
		_code = FString(UTF8_TO_TCHAR(JavaChars));
		env->ReleaseStringUTFChars(code, JavaChars);

		JavaChars = env->GetStringUTFChars(msg, 0);
		_msg = FString(UTF8_TO_TCHAR(JavaChars));
		env->ReleaseStringUTFChars(msg, JavaChars);
	}

	if (UPicoBlueprintFunctionLibrary::PicoQueryOrderCallback.IsBound())
	{
		UPicoBlueprintFunctionLibrary::PicoQueryOrderCallback.Execute(_code, _msg);
	}
}

extern "C" JNIEXPORT void  JNICALL Java_com_epicgames_ue4_GameActivity_nativePicoEntitlementVerifyCallback(JNIEnv * env, jclass clazz, int32 code)
{
	if (UPicoBlueprintFunctionLibrary::PicoEntitlementVerifyCallback.IsBound())
	{
		UPicoBlueprintFunctionLibrary::PicoEntitlementVerifyCallback.Execute(code);
	}
}

#endif