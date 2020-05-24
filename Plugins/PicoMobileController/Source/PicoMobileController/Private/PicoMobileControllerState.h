#pragma once

#include "PicoMobileControllerPrivatePCH.h"

#if PLATFORM_ANDROID

class PicoMobileControllerState
{
public:
	enum ArmJoint {
		Controller = 0,
		Wrist,
		Elbow,
		Shoulder
	};

	ArmJoint ArmJointType;

public:
	static PicoMobileControllerState* GetInstance()
	{
		if (pControllerState == nullptr)
		{
			pControllerState = new PicoMobileControllerState();
		}
		return pControllerState;
	}

	void Destory()
	{
		if (nullptr == pControllerState)
		{
			delete pControllerState;
			pControllerState = nullptr;
		}
	}

public:

	int32 GetHBConnectState();// 0-Disconnect,1-Connect
	bool GetHBButtonState(int32 buttonIndex);
	void SetHBGazeType(int32 gazetype);
	int32 GetHBGazeType();
	void SetHBElbowHeight(float elbowheight);
	void SetHBElbowDepth(float elbowdepth);
	void SetHBPointerTiltAngle(float pointertiltangle);
	void GetHBArmModelParm(FVector &outParm);//The parameter is not a Vector type, just a Vector as a container
	void SetHBArmJoint(int32 armJoint);
	ArmJoint GetHBArmJoint();
	void GetHBOrientation(FQuat &outOri);
	void GetHBPosition(FVector& outPos);
	void GetHBAcc(FVector& outAcc);
	void GetHBAgl(FVector &outAgl);
	void GetHBTouchPad(FVector2D &outTou);
	void GetControllerButtonEvent(int *OutButton);

private:
	PicoMobileControllerState();
	static PicoMobileControllerState* pControllerState;
	~PicoMobileControllerState();

	jmethodID id_getHBConnectState;
	jmethodID id_getHBButtonState;
	jmethodID id_setHBGazeType;
	jmethodID id_getHBGazeType;
	jmethodID id_setHBElbowHeight;
	jmethodID id_setHBElbowDepth;
	jmethodID id_setHBPointerTiltAngle;
	jmethodID id_getHBArmModelParm;
	jmethodID id_setHBArmJoint;
	jmethodID id_getHBOrientation;
	jmethodID id_getHBPosition;
	jmethodID id_getHBAcc;
	jmethodID id_getHBAgl;
	jmethodID id_getHBTouchPad;
	jmethodID id_GetControllerEvent;


};

#endif