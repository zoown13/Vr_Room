// Copyright Pico Technology Co, Ltd. All Rights Reserved.

#pragma once

#include "PicoMobileSettings.generated.h"

UENUM()
namespace EFFRLevel
{
	enum Type
	{
		Off,
		Low,
		Medium,
		High
	};
}

UCLASS(config = Engine, defaultconfig)
class PICOMOBILE_API UPicoMobileSettings
	: public UObject
{
	GENERATED_UCLASS_BODY()

public:

	UPROPERTY(Config, EditAnywhere, Category = PicoMobile, Meta = (DisplayName = "Enable PSensor?", ToolTip = "If you want to get PSensor state,Please enable it!"))
	bool EnablePSensor;

	UPROPERTY(Config, EditAnywhere, Category = PicoMobile, Meta = (DisplayName = "FFR Level"))
	TEnumAsByte<EFFRLevel::Type> FFRLevel;

	UPROPERTY(Config, EditAnywhere, Category = Controller, Meta = (DisplayName = "Enable HomeKey?"))
	bool enableHomeKey;

	UPROPERTY(Config, EditAnywhere, Category = PicoNeo, Meta = (DisplayName = "Only Controller Rotation Tracking"))
	bool disableController6DofModule;

	UPROPERTY(Config, EditAnywhere, Category = PicoNeo, Meta = (DisplayName = "Only HMD Rotation Tracking"))
	bool disableHmd6DofModule;

	UPROPERTY(Config, EditAnywhere, Category = PicoNeo, Meta = (EditCondition = "disableHmd6DofModule", DisplayName = "Enable Neck Model"))
	bool enableNeckModel;

	UPROPERTY(Config, EditAnywhere, Category = PicoNeo, Meta = (EditCondition = "disableHmd6DofModule", DisplayName = "Use Custom NeckParameter"))
	bool useCustomNeckParameter;

	UPROPERTY(Config, EditAnywhere, Category = PicoNeo, Meta = (EditCondition = "useCustomNeckParameter", DisplayName = "NeckOffset"))
	FVector neckOffset;

	UPROPERTY(Config, EditAnywhere, Category = PicoNeo, Meta = (DisplayName = "Enable VR Standard Input Event",ToolTip = "EnableMotionControllerInputEvent"))
		bool enableVRStandardInput;

	UPROPERTY(Config, EditAnywhere, Category = PicoMobile, Meta = (DisplayName = "Enable EyeTracking", ToolTip = "Only Neo2 Eye Supports!"))
		bool EnableEyeTracking;



	// payment
	UPROPERTY(Config, EditAnywhere, Category = Payment, Meta = (DisplayName = "Enable Payment Module"))
	bool enablePaymentModule;

	UPROPERTY(Config, EditAnywhere, Category = Payment, Meta = (DisplayName = "Is Foreign"))
	bool isForeign;

	UPROPERTY(Config, EditAnywhere, Category= Payment, Meta = (DisplayName = "MerchantID"))
	FString merchantID;

	UPROPERTY(Config, EditAnywhere, Category= Payment, Meta = (DisplayName = "AppID"))
	FString appID;

	UPROPERTY(Config, EditAnywhere, Category= Payment, Meta = (DisplayName = "AppKey"))
	FString appKey;

	//UPROPERTY(Config, EditAnywhere, Category= Payment)
	//FString scope;

	UPROPERTY(Config, EditAnywhere, Category= Payment, Meta = (DisplayName = "PayKey"))
	FString payKey;
	
	UPROPERTY(Config, EditAnywhere, Category = Platform, Meta = (DisplayName = "Entitlement Check Simulation[?]", ToolTip = "If true, Development devices will simulate Entitlement Check, you should enter a valid device SN codes list. The SN code can be obtain in Settings-General-Device serial number or input 'adb devices ' in cmd"))
	bool EntitlementCheckSimulation;

	UPROPERTY(Config, EditAnywhere, Category = Platform, Meta = (EditCondition = "EntitlementCheckSimulation", DisplayName = "Device SN Code List"))
	TArray<FString> DeviceSN;

	virtual void PostInitProperties() override;
	
#if WITH_EDITOR
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
#endif // WITH_EDITOR

};
