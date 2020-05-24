#pragma once

#include "TickableObjectRenderThread.h"
#include "GameFramework/PlayerController.h"
#include "Engine/Engine.h"

class FPicoVRSplash : public FTickableObjectRenderThread, public TSharedFromThis<FPicoVRSplash>
{
public:
	FPicoVRSplash(class FPicoMobileHMD* InPicoVRHMD);

	// Registration functions for map load calls
	void RegisterForMapLoad();
	void UnregisterForMapLoad();

	// Map load delegates
	void OnPreLoadMap(const FString&);
	void OnPostLoadMap(UWorld*);

	// FTickableObjectRenderThread overrides
	virtual void Tick(float DeltaTime) override;
	virtual TStatId GetStatId() const override;
	virtual bool IsTickable() const override;

private:
	class FPicoMobileHMD* PicoVRHMD;
};