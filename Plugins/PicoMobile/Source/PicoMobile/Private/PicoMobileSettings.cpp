// Copyright Pico Technology Co, Ltd. All Rights Reserved.

#include "PicoMobileSettings.h"
#include "../Launch/Resources/Version.h"

UPicoMobileSettings::UPicoMobileSettings(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer),
	EnablePSensor(false),
	FFRLevel(EFFRLevel::Off),
	enableHomeKey(true),
	disableController6DofModule(false),
	disableHmd6DofModule(false),
	enableNeckModel(true),
	useCustomNeckParameter(false),
	neckOffset(0, 0.075f, 0.0805f),
	enableVRStandardInput(true),
	EnableEyeTracking(false),
	enablePaymentModule(false),
	isForeign(false),
	EntitlementCheckSimulation(false)
{
}

void UPicoMobileSettings::PostInitProperties()
{
	Super::PostInitProperties();
}

#if WITH_EDITOR
void UPicoMobileSettings::PostEditChangeProperty(FPropertyChangedEvent & PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);
	if (!disableHmd6DofModule ) {
		useCustomNeckParameter = false;
	}
	if (!enableNeckModel) {
		useCustomNeckParameter = false;
	}
	Super::UpdateDefaultConfigFile();
}
#endif // #if WITH_EDITOR

