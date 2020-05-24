// Copyright Pico Technology Co, Ltd. All Rights Reserved.

#pragma once
#include "Kismet/BlueprintFunctionLibrary.h"
#include "PicoBlueprintFunctionLibrary.generated.h"

class UTexture2D;

USTRUCT(BlueprintType, meta = (DisplayName = "PicoCoinOrderInfo"))
struct FPicoCoinOrderInfo
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(BlueprintReadWrite, Category = "PicoPayment")
		FString OrderNumber;

	UPROPERTY(BlueprintReadWrite, Category = "PicoPayment")
		FString OrderTitle;

	UPROPERTY(BlueprintReadWrite, Category = "PicoPayment")
		FString ProductDetail;

	UPROPERTY(BlueprintReadWrite, Category = "PicoPayment")
		FString NotifyUrl;

	UPROPERTY(BlueprintReadWrite, Category = "PicoPayment")
		int32 PicoCoinCount;
};

USTRUCT(BlueprintType, meta = (DisplayName = "PicoCoinOrderInfo"))
struct FPicoPayCodeOrderInfo
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(BlueprintReadWrite, Category = "PicoPayment")
		FString OrderNumber;

	UPROPERTY(BlueprintReadWrite, Category = "PicoPayment")
		FString OrderTitle;

	UPROPERTY(BlueprintReadWrite, Category = "PicoPayment")
		FString ProductDetail;

	UPROPERTY(BlueprintReadWrite, Category = "PicoPayment")
		FString NotifyUrl;

	UPROPERTY(BlueprintReadWrite, Category = "PicoPayment")
		FString PicoPayCode;
};



UENUM(BlueprintType)
enum class FPicoControllerType :uint8
{
	Unknown,
	PicoDisConnectController,
	PicoGoblinController,
	PicoGublin2Controller,
	PicoNeoController,
	PicoNeo2Controller,
};
UENUM(BlueprintType)
enum class PicoFFRLevel :uint8 
{
	Off,
	Low,
	Medium,
	High
};

UENUM(BlueprintType)
enum class EPicoVRHMD_Eye : uint8
{
	LeftEye = 0,
	RightEye,
	AverageEye
};

UENUM(BlueprintType)
enum class EPicoSimulationType : uint8
{
	Null = 0,
	Invalid,
	Valid
};

// Declare delegate
DECLARE_DYNAMIC_DELEGATE_TwoParams(FPicoLogInOutCallbackDelegate, const bool, isSuccess, const FString&, reason);
DECLARE_DYNAMIC_DELEGATE_OneParam(FPicoGetUserInfoCallbackDelegate, const FString&, info);
DECLARE_DYNAMIC_DELEGATE_TwoParams(FPicoPayOrderCallbackDelegate, const FString&, code, const FString&, msg);
DECLARE_DYNAMIC_DELEGATE_TwoParams(FPicoQueryOrderCallbackDelegate, const FString&, code, const FString&, msg);
DECLARE_DYNAMIC_DELEGATE_OneParam(FPicoPaymentExceptionCallbackDelegate, const FString&, exception);
DECLARE_DYNAMIC_DELEGATE_OneParam(FPicoEntitlementVerifyCallbackDelegate, const int32&, code);

// Blueprint interface
UCLASS()
class PICOMOBILE_API UPicoBlueprintFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()


public:

	static FPicoLogInOutCallbackDelegate PicoLogInOutCallback;
	static FPicoGetUserInfoCallbackDelegate PicoGetUserInfoCallback;
	static FPicoPayOrderCallbackDelegate PicoPayOrderCallback;
	static FPicoQueryOrderCallbackDelegate PicoQueryOrderCallback;
	static FPicoPaymentExceptionCallbackDelegate PicoPaymentExceptionCallback;
	static FPicoEntitlementVerifyCallbackDelegate PicoEntitlementVerifyCallback;
public:
	// Sensor
	UFUNCTION(BlueprintCallable, Category = "PicoMobile")
		static FQuat PicoGetCurrentOrientation();

	UFUNCTION(BlueprintCallable, Category = "PicoMobile")
		static FVector PicoGetCurrentPosition();

	UFUNCTION(BlueprintCallable, Category = "PicoMobile")
		static void PicoIs6Dof(bool& is6dof);

	UFUNCTION(BlueprintCallable, Category = "PicoMobile")
		static void PicoGetControllerDeviceType(FPicoControllerType& DeviceType);


	/**
	* You must check the "Enable PSensor ?" form Project setting befor use this function.
	* 0 - near the sensor, 1 - far the sensor , -1 - You do not check "Enable PSensor".
	*/
	UFUNCTION(BlueprintCallable, Category = "PicoMobile")
		static void PicoGetPSensorState(int32& SensorState);

	UFUNCTION(BlueprintCallable, Category = "PicoMobile")
		static bool ResetSensor();

	// Render
	UFUNCTION(BlueprintCallable, Category = "PicoMobile")
		static float PicoGetFov();
	UFUNCTION(BlueprintCallable, Category = "PicoMobile")
		static float PicoGetVFov();
	UFUNCTION(BlueprintCallable, Category = "PicoMobile")
		static float PicoGetHFov();

	// System
	// Brightness,0~255
	UFUNCTION(BlueprintCallable, Category = "PicoMobile")
		static int32 PicoGetCurrentBrightness();

	UFUNCTION(BlueprintCallable, Category = "PicoMobile")
		static bool PicoSetBrightness(int32 Brightness);

	UFUNCTION(BlueprintCallable, Category = "PicoMobile")
		static bool PicoGetGpuUtilization(float& GpuUtilization);

	// Volume
	UFUNCTION(BlueprintCallable, Category = "PicoMobile")
		static int32 PicoGetCurrentVolume();

	UFUNCTION(BlueprintCallable, Category = "PicoMobile")
		static int32 PicoGetMaxVolumeNumber();

	UFUNCTION(BlueprintCallable, Category = "PicoMobile")
		static void PicoSetVolume(int32 Volume);

	UFUNCTION(BlueprintCallable, Category = "PicoMobile")
		static void PicoVolumeUp();

	UFUNCTION(BlueprintCallable, Category = "PicoMobile")
		static void PicoVolumeDown();

	UFUNCTION(BlueprintCallable, Category = "PicoMobile")
		static int32 PicoGetHandness();

	// SN
	UFUNCTION(BlueprintCallable, Category = "PicoMobile")
		static void PicoGetDeviceSN(FString& SN);

	// Set CPU GPU level
	UFUNCTION(BlueprintCallable, Category = "PicoMobile")
		static void SetCPUAndGPULevels(int CPULevel, int GPULevel);

	UFUNCTION(BlueprintCallable, Category = "PicoMobile")
		static bool PicoControllerHomekey();
	// Boundary

	UFUNCTION(BlueprintCallable, Category = "PicoMobile")
		static bool PicoGetBoundaryConfigured();

	UFUNCTION(BlueprintCallable, Category = "PicoMobile")
		static bool PicoGetBoundaryEnabled();

	UFUNCTION(BlueprintCallable, Category = "PicoMobile")
		static void PicoSetBoundaryVisible(bool enable);

	// DeviceModel
	UFUNCTION(BlueprintCallable, Category = "PicoMobile")
		static void PicoGetDeviceModel(FString& Model);

	UFUNCTION(BlueprintCallable, Category = "PicoMobile")
		static EPicoSimulationType PicoIsCurrentDeviceValid();

	UFUNCTION(BlueprintCallable, Category = "PicoMobile")
		static bool PicoAppEntitlementCheck(FString appID);

	UFUNCTION(BlueprintCallable, Category = "PicoMobile")
		static bool PicoKeyEntitlementCheck(FString publicKey);

	UFUNCTION(BlueprintCallable, Category = "PicoMobile")
		static void PicoEntitlementVerifySetCallbackDelegates(
			FPicoEntitlementVerifyCallbackDelegate OnPicoEntitlementVerifyCallback);

	// Payment
	UFUNCTION(BlueprintCallable, Category = "PicoPayment")
		static void PicoPaymentSetCallbackDelegates(
			FPicoLogInOutCallbackDelegate OnPicoLogInOutCallback,
			FPicoGetUserInfoCallbackDelegate OnPicoGetUserInfoCallback,
			FPicoPayOrderCallbackDelegate OnPicoPayOrderCallback,
			FPicoQueryOrderCallbackDelegate OnPicoQueryOrderCallback,
			FPicoPaymentExceptionCallbackDelegate OnPicoPaymentExceptionCallback
		);

	UFUNCTION(BlueprintCallable, Category = "PicoLoginOrLogout")
		static void PicoLoginSDK();

	UFUNCTION(BlueprintCallable, Category = "PicoPayment")
		static void PicoPaymentLogin();

	UFUNCTION(BlueprintCallable, Category = "PicoLoginOrLogout")
		static void PicoLogoutSDK();

	UFUNCTION(BlueprintCallable, Category = "PicoPayment")
		static void PicoPaymentLogout();

	UFUNCTION(BlueprintCallable, Category = "PicoPayment")
		static void PicoSDKGetUserInfo();

	//return Json
	UFUNCTION(BlueprintCallable, Category = "PicoPayment")
		static void PicoPaymentGetUserInfo();

	UFUNCTION(BlueprintCallable, Category = "PicoPayment")
		static void PicoPaymentPayWithCoin(const FPicoCoinOrderInfo& Order);

	UFUNCTION(BlueprintCallable, Category = "PicoPayment")
		static void PicoPaymentPayWithPayCode(const FPicoPayCodeOrderInfo& Order);

	//return Json
	UFUNCTION(BlueprintCallable, Category = "PicoPayment")
	static void PicoPaymentQueryOrder(const FString& OrderNumber);

	UFUNCTION(BlueprintCallable)
		static void LoadMesh(FString FilePathAndName, TArray<FVector> &Vertices,  TArray<int32> &Triangles, TArray<FVector> &Normals, TArray<FVector2D> &UV, FString &Name,bool &Succeed);

	UFUNCTION(BlueprintCallable)
		static void LoadTexture(FString FilePathAndName, UTexture2D* &Texture ,bool &Succeed);

	static TArray<FColor> uint8ToFColor(const TArray<uint8> origin);

	static UTexture2D* TextureFromImage(const int32 SrcWidth, const int32 SrcHeight, const TArray<FColor> &SrcData, const bool UseAlpha);

	 static FString GetRootPath();

	 UFUNCTION(BlueprintCallable, Category = "PicoMobile")
		 static bool PicoGetFFRLevel(PicoFFRLevel& FFRLevel);

	 UFUNCTION(BlueprintCallable, Category = "PicoMobile")
		 static bool PicoSetFFRLevel(PicoFFRLevel FFRLevel);

	 UFUNCTION(BlueprintCallable, Category = "PicoMobile")
		 static bool PicoGetFFRParameter(FVector2D &FoveationGainValue, float &FoveationAreaValue, float &FoveationMinimumValue);
	 
	 UFUNCTION(BlueprintCallable, Category = "PicoMobile")
		 static bool PicoSetFFRParameter(FVector2D FoveationGainValue, float FoveationAreaValue, float FoveationMinimumValue);

	 UFUNCTION(BlueprintCallable, Category = "PicoMobile")
		 static bool PicoGetCurrentDisplayFrequency(float &CurrentDisplayFrequency);

#pragma region Eyetracking
	 // Eye Tracking
	 UFUNCTION(BlueprintCallable, Category = "PicoMobile")
		 static bool PicoGetEyeTrackingGazeRay(FVector& Origin, FVector& Direction);

	 /*UFUNCTION(BlueprintCallable, Category = "PicoMobile")
		 static bool PicoGetEyeDirection(FVector &OutEyeDirection);

	 UFUNCTION(BlueprintCallable, Category = "PicoMobile")
		 static void PicoGetEyeOrientationAndPosition(FVector& OutPosition, FQuat& OutOrientation, FRotator& OutRotation, bool& Success);

	 UFUNCTION(BlueprintCallable, Category = "PicoMobile")
		 static void PicoGetEyeOpenness(EPicoVRHMD_Eye WhichEye, float& Openness, bool& Success);

	 UFUNCTION(BlueprintCallable, Category = "PicoMobile")
		 static void PicoGetPupilDialation(EPicoVRHMD_Eye WhichEye, float& Openness, bool& Success);

	 UFUNCTION(BlueprintCallable, Category = "PicoMobile")
		 static void PicoIsEyetrackingEnabled(bool& IsEnabled);*/

#pragma endregion EyeTracking

};