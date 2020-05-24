/*
* Copyright Pico Technology Co, Ltd. All Rights Reserved.
*/

#pragma once
#include "PicoNeoControllerPrivatePCH.h"
#if PLATFORM_ANDROID

class FPicoNeoControllerJni
{
public:
	static FPicoNeoControllerJni* GetInstance()
	{
		if (pSingleInstance == nullptr)
		{
			pSingleInstance = new FPicoNeoControllerJni();
		}
		return pSingleInstance;
	}

	void Destory()
	{
		if (nullptr == pSingleInstance)
		{
			delete pSingleInstance;
			pSingleInstance = nullptr;
		}
	}

	int32 GetNeoDeviceType();//CV 0 or CV2 4

	void SetMainArmJoint(int32 aj);
	int32 GetMainArmJoint();
	void SetMainGazeType(int32 gt);
	int32 GetMainGazeType();
	void SetMainArmModelParm(float eh, float ed, float pta);
	void GetMainArmModelParm(FVector& AMParm);
	void SetSubArmJoint(int32 aj);
	int32 GetSubArmJoint();
	void SetSubGazeType(int32 gt);
	int32 GetSubGazeType();
	void SetSubArmModelParm(float eh, float ed, float pta);
	void GetSubArmModelParm(FVector& AMParm);

	int32 GetMainControllerConnectState();// 0-Disconnect,1-Connect
	int32 GetSubControllerConnectState();// 0-Disconnect,1-Connect
	int32 GetMainControllerSerialNum();
	int32 GetSubControllerSerialNum();
	int32 GetMainController6DofAbility();// 0-3,1-6
	int32 GetSubController6DofAbility();// 0-3,1-6
	bool GetMainControllerButtonState(int32 buttonIndex);// 0-home,1-app,2-touchpad,3-volUp,4-volDown;false-up,true-down
	bool GetSubControllerButtonState(int32 buttonIndex);// 0-home,1-app,2-touchpad,3-volUp,4-volDown;false-up,true-down
	int32 GetMainControllerBatteryLevel();// 1-10
	int32 GetSubControllerBatteryLevel();// 1-10
	void GetMainControllerOrientation(FQuat& outOri);
	void GetSubControllerOrientation(FQuat& outOri);
	void GetMainControllerPosition(FVector& outPos);
	void GetSubControllerPosition(FVector& outPos);
	void GetMainControllerAcc(FVector& outAcc);
	void GetSubControllerAcc(FVector& outAcc);
	void GetMainControllerAgl(FVector& outAgl);
	void GetSubControllerAgl(FVector& outAgl);
	void GetLeftControllerLvc(FVector& outLvc);
	void GetRightControllerLvc(FVector& outLvc);
	float GetMainControllerTriggerNum();
	float GetSubControllerTriggerNum();
	void GetMainControllerTouchPad(FVector2D& outTou);
	void GetLeftControllerRocker(FVector2D& outRoc); 
	void GetSubControllerTouchPad(FVector2D& outTou);
	void GetRightControllerRocker(FVector2D& outRoc);
	void ShakeMainController();
	void ShakeSubController();
	void ShakeCV2Controller(float strength,int32 time,int32 hand);
	int32 GetMainControllerValue();
	void StartControllerThread();

	void GetControllerButtonEvent(int controller, int* OutButton);

private:
	FPicoNeoControllerJni();
	static FPicoNeoControllerJni* pSingleInstance;
	~FPicoNeoControllerJni();

	jmethodID id_getNeoDeviceType;

	jmethodID id_setMainArmJoint;
	jmethodID id_getMainArmJoint;
	jmethodID id_setMainGazeType;
	jmethodID id_getMainGazeType;
	jmethodID id_setMainArmModelParm;
	jmethodID id_getMainArmModelParm;
	jmethodID id_setSubArmJoint;
	jmethodID id_getSubArmJoint;
	jmethodID id_setSubGazeType;
	jmethodID id_getSubGazeType;
	jmethodID id_setSubArmModelParm;
	jmethodID id_getSubArmModelParm;

	jmethodID id_getMainControllerConnectState;
	jmethodID id_getSubControllerConnectState;
	jmethodID id_getMainControllerSerialNum;
	jmethodID id_getSubControllerSerialNum;
	jmethodID id_getMainController6DofAbility;
	jmethodID id_getSubController6DofAbility;
	jmethodID id_getMainControllerButtonState;
	jmethodID id_getSubControllerButtonState;
	jmethodID id_getMainControllerBatteryLevel;
	jmethodID id_getSubControllerBatteryLevel;
	jmethodID id_getMainControllerOrientation;
	jmethodID id_getSubControllerOrientation;
	jmethodID id_getMainControllerPosition;
	jmethodID id_getSubControllerPosition;
	jmethodID id_getMainControllerAcc;
	jmethodID id_getSubControllerAcc;
	jmethodID id_getMainControllerAgl;
	jmethodID id_getSubControllerAgl;
	jmethodID id_getLeftControllerLvc;
	jmethodID id_getRightControllerLvc;
	jmethodID id_getMainControllerTriggerNum;
	jmethodID id_getSubControllerTriggerNum;
	jmethodID id_getMainControllerTouchPad;
	jmethodID id_getSubControllerTouchPad;
	jmethodID id_shakeMainController;
	jmethodID id_shakeSubController;
	jmethodID id_shakeCV2Controller;
	jmethodID id_getMainControllerValue;
	jmethodID id_StartControllerThread;

	jmethodID id_GetControllerEvent;
};

#endif