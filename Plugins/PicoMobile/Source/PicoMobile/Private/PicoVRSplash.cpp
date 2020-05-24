
#include "PicoVRSplash.h"
#include "PicoMobile.h"
#include "PicoImport.h"


FPicoVRSplash::FPicoVRSplash(class FPicoMobileHMD* InPicoVRHMD)
	: FTickableObjectRenderThread(false, true)
	, PicoVRHMD(InPicoVRHMD)
{}

void FPicoVRSplash::RegisterForMapLoad()
{
	FCoreUObjectDelegates::PreLoadMap.AddSP(this, &FPicoVRSplash::OnPreLoadMap);
	FCoreUObjectDelegates::PostLoadMapWithWorld.AddSP(this, &FPicoVRSplash::OnPostLoadMap);
}

void FPicoVRSplash::UnregisterForMapLoad()
{
	FCoreUObjectDelegates::PreLoadMap.RemoveAll(this);
	FCoreUObjectDelegates::PostLoadMapWithWorld.RemoveAll(this);
};

void FPicoVRSplash::OnPreLoadMap(const FString&){}

void FPicoVRSplash::OnPostLoadMap(UWorld*)
{
#if PLATFORM_ANDROID
	PicoSDK::GIsMapPost = true;
#endif
}

void FPicoVRSplash::Tick(float DeltaTime){}

TStatId FPicoVRSplash::GetStatId() const
{
	RETURN_QUICK_DECLARE_CYCLE_STAT(FPicoVRSplash, STATGROUP_Tickables);
}

bool FPicoVRSplash::IsTickable() const
{
	return true;
}