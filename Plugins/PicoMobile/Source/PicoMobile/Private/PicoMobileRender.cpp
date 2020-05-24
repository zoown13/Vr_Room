#include "PicoMobile.h"

#include "PicoImport.h"
#include "PicoMobileSettings.h"
#include "PicoJava.h"
#include "PicoEyeTracker.h"

DEFINE_LOG_CATEGORY_STATIC(LogPicoRender, Log, All);

#if PLATFORM_ANDROID
FPicoMobileHMDTextureSet::FPicoMobileHMDTextureSet(
	class FOpenGLDynamicRHI* InGLRHI,
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
	uint32 InTargetableTextureFlags
) : FOpenGLTexture2D(
	InGLRHI,
	0,
	InTarget,
	InAttachment,
	InSizeX,
	InSizeY,
	InSizeZ,
	InNumMips,
	InNumSamples,
	InNumSamplesTileMem,
	InArraySize,
	InFormat,
	bInCubemap,
	bInAllocatedStorage,
	InFlags,
	InTextureRange,
	FClearValueBinding::Black)
{
	FRHIResourceCreateInfo CreateInfo;
	bool bArrayTexture = (InArraySize > 1);
	for (int i = 0; i < mkRenderTargetTextureNum; ++i)
	{
		GLuint TextureID = 0;
		FOpenGL::GenTextures(1, &TextureID);

		const bool bSRGB = (InTargetableTextureFlags & TexCreate_SRGB) != 0;
		const FOpenGLTextureFormat& GLFormat = GOpenGLTextureFormats[InFormat];
		if (GLFormat.InternalFormat[bSRGB] == GL_NONE)
		{
			UE_LOG(LogPicoRender, Fatal, TEXT("Pvr_UE Texture format '%s' not supported (sRGB=%d)."), GPixelFormats[InFormat].Name, bSRGB);
		}

		FOpenGLContextState& ContextState = InGLRHI->GetContextStateForCurrentContext();

		// Make sure PBO is disabled
		InGLRHI->CachedBindPixelUnpackBuffer(ContextState, 0);

		// Use a texture stage that's not likely to be used for draws, to avoid waiting
		InGLRHI->CachedSetupTextureStage(ContextState, FOpenGL::GetMaxCombinedTextureImageUnits() - 1, InTarget, TextureID, 0, 1);

		if (bArrayTexture)
		{
			UE_LOG(LogPicoRender,Log,TEXT("FOpenGL::TexStorage3D"));
			FOpenGL::TexStorage3D(InTarget, 1, GLFormat.InternalFormat[bSRGB], InSizeX, InSizeY, InArraySize, GLFormat.Format, GLFormat.Type);
		}
		else if (!FOpenGL::TexStorage2D(InTarget, 1, GLFormat.SizedInternalFormat[bSRGB], InSizeX, InSizeY, GLFormat.Format, GLFormat.Type, InTargetableTextureFlags))
		{
			glTexImage2D(InTarget, 0, GLFormat.InternalFormat[bSRGB], InSizeX, InSizeY, 0, GLFormat.Format, GLFormat.Type, NULL);
		}

		if (bArrayTexture)
		{
			mRenderTargetTextureRHIRef[i] = InGLRHI->RHICreateTexture2DArrayFromResource((EPixelFormat)InFormat, InSizeX, InSizeY, InArraySize, InNumMips, InNumSamples, InNumSamplesTileMem, FClearValueBinding::Black, TextureID, InTargetableTextureFlags);
		}
		else
		{
			mRenderTargetTextureRHIRef[i] = InGLRHI->RHICreateTexture2D(InSizeX, InSizeY, (EPixelFormat)InFormat, InNumMips, InNumSamples, InTargetableTextureFlags, CreateInfo);
		}
	}
	mRenderTargetTextureRHIRefIndex = 0;
	SetIndexAsCurrentRenderTarget();
}

FPicoMobileHMDTextureSet* FPicoMobileHMDTextureSet::CreateTexture2DSet(
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
	FTexture2DRHIRef& OutShaderResourceTexture)
{
	FPicoMobileHMDTextureSet* TextureSetProxy = new FPicoMobileHMDTextureSet(InGLRHI, InTarget, GL_NONE, SizeX, SizeY, 0, 1, InNumSamples, InNumSamplesTileMem, InArraySize, InFormat, false, false, InFlags, nullptr, InTargetableTextureFlags);
	OutTargetableTexture = OutShaderResourceTexture = TextureSetProxy->GetTexture2D();
	return TextureSetProxy;
}

FPicoMobileHMDTextureSet::~FPicoMobileHMDTextureSet() 
{
	this->SetMemorySize(0);
}

void FPicoMobileHMDTextureSet::SwitchToNextElement()
{
	mRenderTargetTextureRHIRefIndex = ((mRenderTargetTextureRHIRefIndex + 1) % mkRenderTargetTextureNum);
	SetIndexAsCurrentRenderTarget();
}

void FPicoMobileHMDTextureSet::SetIndexAsCurrentRenderTarget()
{
	Resource = GetRenderTargetResource();
}

GLuint FPicoMobileHMDTextureSet::GetRenderTargetResource()
{
	return *(GLuint*)mRenderTargetTextureRHIRef[mRenderTargetTextureRHIRefIndex]->GetNativeResource();
}

#endif

#if ENGINE_MINOR_VERSION > 22
void FPicoMobileHMD::RenderTexture_RenderThread(FRHICommandListImmediate& RHICmdList, FRHITexture2D* BackBuffer, FRHITexture2D* SrcTexture, FVector2D WindowSize) const
#else
void FPicoMobileHMD::RenderTexture_RenderThread(FRHICommandListImmediate& RHICmdList, FTexture2DRHIParamRef BackBuffer, FTexture2DRHIParamRef SrcTexture, FVector2D WindowSize) const
#endif
{
	check(IsInRenderingThread());
	FinishRendering();// texture to so
}

bool FPicoMobileHMD::AllocateRenderTargetTexture(uint32 Index, uint32 SizeX, uint32 SizeY, uint8 Format, uint32 NumMips, uint32 InFlags, uint32 TargetableTextureFlags, FTexture2DRHIRef& OutTargetableTexture, FTexture2DRHIRef& OutShaderResourceTexture, uint32 NumSamples)
{
#if PLATFORM_ANDROID
	static const int32 MaxMSAALevelSupported = FOpenGL::GetMaxMSAASamplesTileMem();
	static const int32 MSAASettingLevel = IConsoleManager::Get().FindTConsoleVariableDataInt(TEXT("r.MobileMSAA"))->GetValueOnAnyThread();
	static const int32 MobileMSAAValue = FMath::Min(MSAASettingLevel, MaxMSAALevelSupported);
	

	GLenum Target = GL_TEXTURE_2D;
	uint32 Layers = 1;
	if (bUsesArrayTexture)
	{
		Target = GL_TEXTURE_2D_ARRAY;
		Layers = 2;
	}
	uint32 Width = PicoSDK::Render::GTextureSize.X;
	auto OpenGLDynamicRHI = static_cast<FOpenGLDynamicRHI*>(GDynamicRHI);
	check(OpenGLDynamicRHI);
	mTextureSet = TRefCountPtr<FPicoMobileHMDTextureSet>(FPicoMobileHMDTextureSet::CreateTexture2DSet(OpenGLDynamicRHI, Target, Width, SizeY, NumSamples, MobileMSAAValue, Layers, EPixelFormat(Format), InFlags, TargetableTextureFlags, OutTargetableTexture, OutShaderResourceTexture));
	return true;
#endif 
	return false;
}

void FPicoMobileHMD::FinishRendering()const
{
#if PLATFORM_ANDROID

	auto RTTextureId = mTextureSet->GetRenderTargetResource();

	if (PicoSDK::Render::FFRLevel != 0)
	{
		FVector EyeDirection = FVector::ZeroVector;
		if (!EyeTracker->GetEyeDirectionToFFR(EyeDirection))
		{
			UE_LOG(LogPicoRender, Log, TEXT("Pvr_UE GetEyeDirectionToFFR Failed!"));
		}
		PicoSDK::Render::SetFFRParameters(RTTextureId, PicoSDK::Render::PreviousId, EyeDirection);
		PicoSDK::Render::PreviousId = RTTextureId;
		PicoSDK::Render::SendFFREvent();
	}
	if (bUsesArrayTexture)
	{
		PicoSDK::Render::RenderEventWithData(EVENT_BOTHEYE_ENDFRAME, RTTextureId);
	}
	else
	{
		PicoSDK::Render::RenderEventWithData(EVENT_LEFTEYE_ENDFRAME, RTTextureId);
	}
	
	int32 num = PicoSDK::Sensor::GViewnumber;
	
	//boundary
	PicoSDK::Render::RenderEventWithData(EVENT_BOUNDARY_LEFT, num);
	if (!bUsesArrayTexture)
	{
		PicoSDK::Render::RenderEventWithData(EVENT_BOUNDARY_RIGHT, num);
	}
	
	PicoSDK::Render::RenderEventWithData(EVENT_TIMEWARP, num);

	mTextureSet->SwitchToNextElement();

#if UE_BUILD_DEBUG || UE_BUILD_DEVELOPMENT
	// frame rate log
	static int count = 0;
	static double theTime = 0.0f;
	if (count == 0)
	{
		theTime = FPlatformTime::Seconds();
	}
	count++;
	{
		double NewTime = FPlatformTime::Seconds();
		double deltaTime = NewTime - theTime;
		if (deltaTime > 1.0f)
		{
			UE_LOG(LogTemp, Warning, TEXT("Pvr_UE FPS : %d "), (int)(count / deltaTime));
			theTime = NewTime;
			count = 0;
		}
	}
#endif
#endif
}