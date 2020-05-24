// Copyright Pico Technology Co, Ltd. All Rights Reserved.

#pragma once

#include "../Launch/Resources/Version.h"
#include "Engine.h"
#include "HAL/Platform.h"


#if PLATFORM_ANDROID
#include "OpenGLDrvPrivate.h"
#include "OpenGLResources.h"
#endif


#if PLATFORM_ANDROID
#include <dlfcn.h>

// Sensors
typedef int32(*FDllPvrInitProc)(int32 index);
typedef int32(*FDllPvrStartSensorProc)(int32 index);
typedef int32(*FDllPvrStopSensorProc)(int32 index);
typedef int32(*FDllPvrResetSensorProc)(int32 index);
typedef int32(*FDllPvrOptionalResetSensorProc)(int32 index, int32 resetRot, int32 resetPos);

typedef int(*FDllPvrEnable6DofModuleProc)(bool enable);

typedef int32(*FDllPvrGetMainSensorStateProc)(float &x, float &y, float &z, float &w, float &px, float &py, float &pz, float &hfov,float &vfov, int32 &viewNumber);
typedef int32(*FDllPvrGetSensorStateProc)(int32 index, float &x, float &y, float &z, float &w, float &px, float &py, float &pz);
typedef int32(*FDllPvrGetPsensorStateProc)();

typedef bool(*FDllPvrGetAccelerationProc)(float &x, float &y, float &z);
typedef bool(*FDllPvrGetAngularVelocityProc)(float &x, float &y, float &z);

// Render
typedef void(*FDllPvrSetAsyncTimeWarpProc)(bool enable);
typedef void(*FDllPvrSetVsyncFrameRateProc)(double rate);

typedef void(*FDllPvrSetPupillaryPointProc)(bool enable);
typedef void(*FDllPvrChangeHeadwearProc)(int32 type);
typedef void(*FDllPvrChangeScreenParametersProc)(const char* model, int32 width, int32 height, double xppi, double yppi, double densityDpi);
typedef void(*FDllPvrGetFOVProc)(float &fov);
typedef void(*FDllPvrSetUnrealParamProc)(int32 UnrealParamKey, int32 value);

typedef void(*FDllPvrSetCPUGPULevelsProc)(int32 CPULevel, int32 GPUlevel);
//FFR
typedef void(*FDllPvrSetFoveationParametersProc)(int textureId, int previousId, float focalPointX, float focalPointY, float foveationGainX, float foveationGainY, float foveationArea, float foveationMinimum);

typedef bool(*FDllPvrGetAsyncFlagProc)();

typedef void(*FDllPvrGetRendertextureSizeProc)(int32 &W, int32 &H);
typedef void(*FDllPvrGetIntConfigProc)(int32 configsenum, int32 &res);
typedef void(*FDllPvrGetFloatConfigProc)(int32 configsenum, float &res);

//MultiView
typedef void(*FDllPvrEnableMultiViewProc)(bool enable);

// System
typedef void(*FDllPvrSetInitActivityProc)(jobject activity, jclass vrActivityClass);
typedef char* (*FDllPvrGetSDKVersionProc)();
typedef float(*FDllPvrGetGpuUtilizationProc)();

//Boundary
typedef int32(*FDllPvrGetSeeThroughStateProc)();
typedef void(*FDllPvrSubmitRenderTargetSizeProc)(int32 RTX, int32 RTY);
typedef bool(*FDllPvrSetTrackingOriginProc)(int32 trackingOrigin);
typedef bool(*FDllPvrGetFrameRateLimitProc)();
typedef bool(*FDllPvrGetBoundaryConfiguredProc)();
typedef bool(*FDllPvrGetBoundaryEnabledProc)();
typedef void(*FDllPvrSetBoundaryVisibleProc)(bool enable);
typedef void(*FDllPvrSetReinPositionProc)(float x, float y, float z, float w, float px, float py, float pz, int hand, bool valid, int keyEvent);

// Plugin
typedef void(*FDllPvrSpatializerUnlockProc)(const char* s);
typedef void(*FDllPvrUnityRenderEventProc)(int32 id);
typedef void(*FDllPvrSetOverlayModelViewMatrixProc)(int32 layerType,int imageType,int texID,int eye,int layerIndex,bool fixed_overlay,int layerFlags,
	float mvMatrix[],float scaleM[],float rotationM[],float translationM[],float rotationC[],float translationC[]);

//EyeTracking
typedef bool(*FDllPvrGetEyeTrackingDataProc)(int32& leftEyePoseStatus,int32& rightEyePoseStatus,int32& combinedEyePoseStatus,
	float &leftEyeGazePointX,float& leftEyeGazePointY,float& leftEyeGazePointZ,
	float &rightEyeGazePointX,float& rightEyeGazePointY, float &rightEyeGazePointZ,
	float &combinedEyeGazePointX,float& combinedEyeGazePointY,float& combinedEyeGazePointZ,
	float &leftEyeGazeVectorX,float &leftEyeGazeVectorY,float &leftEyeGazeVectorZ,
	float &rightEyeGazeVectorX,float &rightEyeGazeVectorY,float &rightEyeGazeVectorZ,
	float &combinedEyeGazeVectorX,float &combinedEyeGazeVectorY,float &combinedEyeGazeVectorZ,
	float &leftEyeOpenness,float &rightEyeOpenness,
	float &leftEyePupilDilation,float &rightEyePupilDilation,
	float &leftEyePositionGuideX,float &leftEyePositionGuideY,float &leftEyePositionGuideZ,
	float &rightEyePositionGuideX,float &rightEyePositionGuideY,float &rightEyePositionGuideZ);
typedef int32(*FDllPvrGetTrackingModeProc)();

typedef bool(*FDllPvrSetTrackingModeProc)(int trackingMode);


class PicoImport
{
public:
	static bool Init();
	static void Uninit();

	static FString GetPluginDir();

	static FDllPvrInitProc PvrInit;
	static FDllPvrStartSensorProc PvrStartSensor;
	static FDllPvrStopSensorProc PvrStopSensor;
	static FDllPvrResetSensorProc PvrResetSensor;
	static FDllPvrOptionalResetSensorProc PvrOptionalResetSensor;

	static FDllPvrEnable6DofModuleProc PvrEnable6DofModule;

	static FDllPvrGetMainSensorStateProc PvrGetMainSensorState;
	static FDllPvrGetSensorStateProc PvrGetSensorState;
	static FDllPvrGetPsensorStateProc PvrGetPsensorState;

	static FDllPvrGetAccelerationProc PvrGetAcceleration;
	static FDllPvrGetAngularVelocityProc PvrGetAngularVelocity;

	static FDllPvrSetUnrealParamProc PvrSetUnrealParam;
	static FDllPvrSetCPUGPULevelsProc PvrSetCPUGPULevels;
	static FDllPvrSetTrackingOriginProc PvrSetTrackingOrigin;
	static FDllPvrGetFrameRateLimitProc PvrGetFrameRateLimit;

	static FDllPvrGetRendertextureSizeProc PvrGetRendertextureSize;
	static FDllPvrGetIntConfigProc PvrGetIntConfig;
	static FDllPvrGetFloatConfigProc PvrGetFloatConfig;

	static FDllPvrSetAsyncTimeWarpProc PvrSetAsyncTimeWarp;
	static FDllPvrSetVsyncFrameRateProc PvrSetVsyncFrameRate;

	static FDllPvrSetPupillaryPointProc PvrSetPupillaryPoint;
	static FDllPvrChangeHeadwearProc PvrChangeHeadwear;
	static FDllPvrChangeScreenParametersProc PvrChangeScreenParameters;
	static FDllPvrGetFOVProc PvrGetFOV;


	//FFR
	static FDllPvrSetFoveationParametersProc PvrSetFoveationParameters;
	
	//MultiView
	static FDllPvrEnableMultiViewProc PvrEnableMultiView;

	static FDllPvrGetAsyncFlagProc PvrGetAsyncFlag;

	static FDllPvrSetInitActivityProc PvrSetInitActivity;

	static FDllPvrGetSDKVersionProc PvrGetSDKVersion;
	static FDllPvrSpatializerUnlockProc PvrSpatializerUnlock;
	static FDllPvrGetGpuUtilizationProc PvrGetGpuUtilization;

	//Boundary
	static FDllPvrGetSeeThroughStateProc PvrGetSeeThroughState;
	static FDllPvrSubmitRenderTargetSizeProc PvrSubmitRenderTargetSize;
	static FDllPvrUnityRenderEventProc PvrUnityRenderEvent;
	static FDllPvrSetOverlayModelViewMatrixProc PvrSetOverlayModelViewMatrix;
	static FDllPvrGetBoundaryConfiguredProc PvrGetBoundaryConfigured;
	static FDllPvrGetBoundaryEnabledProc PvrGetBoundaryEnabled;
	static FDllPvrSetBoundaryVisibleProc PvrSetBoundaryVisible;
	static FDllPvrSetReinPositionProc PvrSetReinPosition;

	//EyeTracking
	static FDllPvrGetEyeTrackingDataProc PvrGetEyeTrackingData;
	static FDllPvrGetTrackingModeProc  PvrGetTrackingMode;
	static FDllPvrSetTrackingModeProc PvrSetTrackingMode;
protected:
	static void FreeDependency(void*& Handle);
	static bool LoadDependency(const FString& Dir, const FString& Name, void*& Handle);

private:
	static void* GSharedLibraryHandle;
	static FString GPluginDir;
};


struct PicoSDKConfig
{
	enum GlobalIntConfigs
	{
		EYE_TEXTURE_RESOLUTION0,
		EYE_TEXTURE_RESOLUTION1,
		SEENSOR_COUNT,
		ABILITY6DOF,
	};

	enum GlobalFloatConfigs
	{
		IPD,		
		HFOV,
		VFOV,
		NECK_MODEL_X,
		NECK_MODEL_Y,
		NECK_MODEL_Z,
		DISPLAY_REFRESH_RATE
	};
};

enum UnrealParamKeys {
	UNREAL_VERSION = 1,
	BUILD_TYPE = 2,
	USE_NO_ERROR_KHR = 3
};
enum RenderEventType
{
	EVENT_INIT_RENDERTHREAD = 1024,
	EVENT_PAUSE,
	EVENT_RESUME,
	EVENT_LEFTEYE_ENDFRAME,
	EVENT_RIGHTEYE_ENDFRAME,
	EVENT_TIMEWARP,
	EVENT_RESET_VRMODEPARMS,
	EVENT_SHUTDOWN_RENDERTHREAD,
	EVENT_BEGINEYE,  //foveation
	EVENT_ENDEYE,  //foveation
	EVENT_BOUNDARY_LEFT,
	EVENT_BOUNDARY_RIGHT,
	EVENT_BOTHEYE_ENDFRAME, //single pass
	NUM_EVENTS
};


const int32 MAX_SENSOR_COUNT = 2;   // <!--Max sensors number count for now, might be changed.-->


class PicoSDK
{
public:

	class Sensor
	{
	public:
		static int32 GMainSensorIndex;
		static int32 GSensorCount;
		static bool GSensorStart[MAX_SENSOR_COUNT];
		static bool GSensorInit;
		static bool GSensorCanUse;
		static float GFov;
		static float GVFov;
		static float GHFov;
		static FQuat GCurrentOrientation;
		static FVector GCurrentPosition;
		static int32 GViewnumber;
		static bool GIs6DofEnabled;
		static bool EnableNeckModel;
		static FVector NeckOffset;
		static int32 TrackingOrigin;

	public:
		static bool SensorInit();
		static bool GetSensorCount(int32& count);
		static bool StartSensor(int32 sensorIndex);
		static bool StopSensor(int32 sensorIndex);
		static bool ResetSensor(int32 sensorIndex);
		static bool OptionalResetSensor(int32 sensorIndex, int32 isResetRotation, int32 isResetPosition);

		static bool GetSensorState(int32 sensorIndex, FQuat& orientation, FVector& position);
		static bool GetMainSensorState(bool isInRenderThread);
		static bool GetPSensorState(int32& status);// 0 when HMD closed, 1 when HMD is away

		static bool Enable6Dof(bool enable);
		static bool GetAcceleration();
		static bool GetAngularVelocity();
		static void GetNeckOffset();
	};

	class Render
	{
	public:
		static void GetFOV(float& Fov);
		static void GetFOV(float& VFov,float& HFov);
		static void GetIPD(float& Ipd);
		static void GetDisplayRefreshRate(float& RefreshRate);
		static void SetFFRParameters(int textureId, int previousId, FVector EyeDirection);    //FFR
		static void SendFFREvent();
		static GLuint PreviousId;

		static void EnableMultiView(bool isEnable);//Multiview	
		static void RenderThreadInit();
		static void RenderThreadShutdown();
		static void RenderEventWithData(RenderEventType eventType, int32 eventData);
		static void PvrSetUnrealParam(int32 UnrealParamKey, int32 value);
		static void SetCPUGPULevels(int32 CPULevel, int32 GPULevel);
	public:
		static FIntPoint GTextureSize;  //<!-- current texture size , got from config file by mobile SDK -->
		static float DisplayRate;
		static int32 FFRLevel;
		static bool isUseCustomFFRLevel;
		static FVector2D CustomGainValue;
		static float CustomAreaValue;
		static float CustomMinimumValue;
	};

	//SeeThrough
	class SeeThrough
	{
	public:
		static int32 GetSeeThroughState();
		static bool GetBoundaryConfigured();
		static bool GetBoundaryEnabled();
		static void	SetBoundaryVisible(bool enable);
		static void	SetReinPosition();

	public:
		static int32 SeeThroughState;
	};

	static void SendResumeEvent();
	static void SendPauseEvent();
	static bool GIsSDKInit;
	static bool GIsMapPost;
	static uint32 GMapInitFrame;
};
#endif