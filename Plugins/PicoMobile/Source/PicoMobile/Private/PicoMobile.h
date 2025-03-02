// Copyright Pico Technology Co, Ltd. All Rights Reserved.

#pragma once
#include "IPicoMobilePlugin.h"

#include "HeadMountedDisplayBase.h"
#include "IStereoLayers.h"
#include "SceneViewExtension.h"
#include "XRRenderTargetManager.h"
#include "PicoVRSplash.h"
#include "PicoVRStereoLayers.h"

//#include "OpenGLDrvPrivate.h"
//#include "OpenGLResources.h"

#include "../Launch/Resources/Version.h"

#if PLATFORM_ANDROID
#include "Android/AndroidJNI.h"
#include "Android/AndroidApplication.h"


class FPicoMobileHMD;

class FPicoMobileHMDTextureSet :public FOpenGLTexture2D
{
public:
	static FPicoMobileHMDTextureSet* CreateTexture2DSet(
		FOpenGLDynamicRHI* InGLRHI,
		GLenum InTarget,
		uint32 SizeX,
		uint32 SizeY,
		uint32 InNumSamples,
		uint32 InNumSamplesTileMem,
		uint32 InArraySize,
		EPixelFormat InFormat,
		uint32 InFlags,
		uint32 InTargetableTextureFlags,
		FTexture2DRHIRef& OutTargetableTexture,
		FTexture2DRHIRef& OutShaderResourceTexture);

	FPicoMobileHMDTextureSet(
		class FOpenGLDynamicRHI* InGLRHI,
		//GLuint InResource,
		GLenum InTarget,
		GLenum InAttachment,
		uint32 InSizeX,
		uint32 InSizeY,
		uint32 InSizeZ,
		uint32 InNumMips,
		uint32 InNumSamples,
		uint32 InNumSamplesTileMem,
		uint32 InArraySize,
		EPixelFormat InFormat,
		bool bInCubemap,
		bool bInAllocatedStorage,
		uint32 InFlags,
		uint8* InTextureRange,
		uint32 InTargetableTexureFlags);

	virtual ~FPicoMobileHMDTextureSet() override;

	void SwitchToNextElement();
	void SetIndexAsCurrentRenderTarget();
	GLuint GetRenderTargetResource();

private:
	FPicoMobileHMDTextureSet(const FPicoMobileHMDTextureSet &) = delete;
	FPicoMobileHMDTextureSet(FPicoMobileHMDTextureSet &&) = delete;
	FPicoMobileHMDTextureSet &operator=(const FPicoMobileHMDTextureSet &) = delete;

	enum { mkRenderTargetTextureNum = 3 };
	FTextureRHIRef    mRenderTargetTextureRHIRef[mkRenderTargetTextureNum];
	int                 mRenderTargetTextureRHIRefIndex;

};
#endif

class FPicoEyeTracker;
class FPicoMobileHMD : public FHeadMountedDisplayBase, public FSceneViewExtensionBase, public FXRRenderTargetManager, public IStereoLayers
{
public:
	FPicoMobileHMD(const FAutoRegister&);
	virtual ~FPicoMobileHMD();

	// IXRSystemIdentifier
	virtual FName GetSystemName() const override;

	// IXRTrackingSystem
	virtual bool DoesSupportPositionalTracking() const override;
	virtual bool EnumerateTrackedDevices(TArray<int32>& OutDevices, EXRTrackedDeviceType Type = EXRTrackedDeviceType::Any) override;
	virtual bool GetCurrentPose(int32 DeviceId, FQuat& OutOrientation, FVector& OutPosition) override;
	virtual void SetTrackingOrigin(EHMDTrackingOrigin::Type NewOrigin) override;
#if ENGINE_MINOR_VERSION > 22
	virtual EHMDTrackingOrigin::Type GetTrackingOrigin() const override;
#else
	virtual EHMDTrackingOrigin::Type GetTrackingOrigin() override;
#endif
	virtual bool GetRelativeEyePose(int32 DeviceId, EStereoscopicPass Eye, FQuat& OutOrientation, FVector& OutPosition) override;
	virtual void ResetOrientationAndPosition(float yaw = 0.f) override;
	virtual void ResetOrientation(float Yaw = 0.f) override;
	virtual void ResetPosition() override;
	virtual void SetBaseRotation(const FRotator& BaseRot) override;
	virtual FRotator GetBaseRotation() const override;
	virtual void SetBaseOrientation(const FQuat& BaseOrient) override;
	virtual FQuat GetBaseOrientation() const override;
	virtual class IHeadMountedDisplay* GetHMDDevice() override;
	virtual class TSharedPtr< class IStereoRendering, ESPMode::ThreadSafe > GetStereoRenderingDevice() override;
	virtual bool OnStartGameFrame(FWorldContext& WorldContext) override;
	virtual void OnBeginRendering_RenderThread(FRHICommandListImmediate& RHICmdList, FSceneViewFamily& ViewFamily) override;

	// IHeadMountedDisplay
	virtual bool IsHMDConnected() override { return true; }
	virtual bool IsHMDEnabled() const override { return true; };
	virtual EHMDWornState::Type GetHMDWornState();
	virtual void EnableHMD(bool allow = true) override;
	virtual bool GetHMDMonitorInfo(MonitorInfo&) override;
	virtual void GetFieldOfView(float& OutHFOVInDegrees, float& OutVFOVInDegrees) const override;
	virtual void GetEyeRenderParams_RenderThread(const struct FRenderingCompositePassContext& Context, FVector2D& EyeToSrcUVScaleValue, FVector2D& EyeToSrcUVOffsetValue)  const override;
	virtual void SetInterpupillaryDistance(float NewInterpupillaryDistance) override;
	virtual float GetInterpupillaryDistance() const override;
	virtual bool GetHMDDistortionEnabled(EShadingPath ShadingPath) const override;
	virtual bool IsChromaAbCorrectionEnabled() const override;
	virtual float GetPixelDenity() const override;
	virtual void SetPixelDensity(const float NewPixelDensity) override;
	virtual FIntPoint GetIdealRenderTargetSize() const override;


	// IStereoRendering
	virtual bool IsStereoEnabled() const override;
	virtual bool EnableStereo(bool stereo = true) override;
	virtual void AdjustViewRect(EStereoscopicPass StereoPass, int32& X, int32& Y, uint32& SizeX, uint32& SizeY) const override;
	virtual FMatrix GetStereoProjectionMatrix(const enum EStereoscopicPass StereoPassType) const override;
#if ENGINE_MINOR_VERSION > 22
	virtual void RenderTexture_RenderThread(FRHICommandListImmediate& RHICmdList, FRHITexture2D* BackBuffer, FRHITexture2D* SrcTexture, FVector2D WindowSize) const override;
#else
	virtual void RenderTexture_RenderThread(FRHICommandListImmediate& RHICmdList, FTexture2DRHIParamRef BackBuffer, FTexture2DRHIParamRef SrcTexture, FVector2D WindowSize) const override;
#endif
	virtual IStereoRenderTargetManager* GetRenderTargetManager() override { return this; }

	virtual IStereoLayers* GetStereoLayers() override { return this; }

	// IStereoLayers interface
	virtual uint32 CreateLayer(const IStereoLayers::FLayerDesc& InLayerDesc) override;
	virtual void DestroyLayer(uint32 LayerId) override;
	virtual void SetLayerDesc(uint32 LayerId, const IStereoLayers::FLayerDesc& InLayerDesc) override;
	virtual bool GetLayerDesc(uint32 LayerId, IStereoLayers::FLayerDesc& OutLayerDesc) override;
	virtual void MarkTextureForUpdate(uint32 LayerId) override {};
	virtual void UpdateSplashScreen() override {};
	virtual IStereoLayers::FLayerDesc GetDebugCanvasLayerDesc(FTextureRHIRef Texture) override { return IStereoLayers::FLayerDesc(); };
	virtual void GetAllocatedTexture(uint32 LayerId, FTextureRHIRef &Texture, FTextureRHIRef &LeftTexture) override {};
#if ENGINE_MINOR_VERSION > 21
	virtual bool ShouldCopyDebugLayersToSpectatorScreen() const override { return true; }
#endif

	// ISceneViewExtension
	virtual void SetupViewFamily(FSceneViewFamily& InViewFamily) override;
	virtual void SetupView(FSceneViewFamily& InViewFamily, FSceneView& InView) override;
	virtual void BeginRenderViewFamily(FSceneViewFamily& InViewFamily) override;
	virtual void PreRenderViewFamily_RenderThread(FRHICommandListImmediate& RHICmdList, FSceneViewFamily& InViewFamily) override;
	virtual void PreRenderView_RenderThread(FRHICommandListImmediate& RHICmdList, FSceneView& InView) override;
	virtual void PostRenderViewFamily_RenderThread(FRHICommandListImmediate& RHICmdList, FSceneViewFamily& InViewFamily) override;
	virtual int32 GetPriority() const override { return -1; }

	// IStereoRenderTargetManager
	virtual bool ShouldUseSeparateRenderTarget() const override;
	virtual void UpdateViewport(bool bUseSeparateRenderTarget, const class FViewport& Viewport, class SViewport* ViewportWidget = nullptr) override;
	virtual void CalculateRenderTargetSize(const class FViewport& Viewport, uint32& InOutSizeX, uint32& InOutSizeY) override;
	virtual bool NeedReAllocateViewportRenderTarget(const FViewport& Viewport) override;
	virtual uint32 GetNumberOfBufferedFrames() const { return 1; }
	virtual bool AllocateRenderTargetTexture(uint32 Index, uint32 SizeX, uint32 SizeY, uint8 Format, uint32 NumMips, uint32 InFlags, uint32 TargetableTextureFlags, FTexture2DRHIRef& OutTargetableTexture, FTexture2DRHIRef& OutShaderResourceTexture, uint32 NumSamples) override;

protected:
	// FXRTrackingSystemBase
	virtual float GetWorldToMetersScale() const override;

public:
	bool IsInitialized() const;

	void RefreshMainSensorState_RenderThread();

private:
	void ConnectToAndroid();
	void SubmitUnrealInfo();
	bool Startup();
	void Shutdown();

	void ApplicationPauseDelegate();
	void ApplicationResumeDelegate();

	void EnterVRMode();
	void LeaveVRMode();

	void FinishRendering()const;
	bool SetPicoRenderTargetSize();
	bool GetMultiViewState();
	void NormalizeRTSize();
	void LimitFrameRate();
	bool isStereoEnabled;

	FQuat		CurHmdOrientation;
	FQuat		LastHmdOrientation;
	FRotator	DeltaControlRotation;
	FQuat		DeltaControlOrientation;

	double	LastSensorTime;

	FQuat		BaseOrientation;
	#if PLATFORM_ANDROID
	TRefCountPtr<FPicoMobileHMDTextureSet> mTextureSet;
#endif

	bool bUsesArrayTexture;

    float FixedFrameRate;
	bool UseFixedFrameRate;
	FPicoEyeTracker*  EyeTracker;
	float PixelDensity;

	TSharedPtr<FPicoVRSplash> PicoSplash;

	void OverlayRendering();
	uint32 NextPicoLayerId;
	TMap<uint32, FPicoVRStereoLayerPtr> PicoLayerMap;
	TMap<uint32, int32> LayerSortIndex;
	TMap<FString, FVector> MapToInitPos;
	bool isFixOverlay;
	FVector InitCamPos;
	FVector PreCamPos;
};