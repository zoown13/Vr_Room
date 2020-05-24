/*
* Copyright Pico Technology Co, Ltd. All Rights Reserved.
*/

#pragma once
#include "../Launch/Resources/Version.h"
#include "IPicoMobilePlugin.h"

#if PLATFORM_ANDROID

#include "Android/AndroidApplication.h"
#include "Android/AndroidJNI.h"


enum EGameStatus {
	NoValue,
	GamePaused,
	GameResumed
};

class FPicoJava
{
	FPicoJava();
	~FPicoJava();

public:
	static bool VerifyAPP(FString appID, FString publicKey);
	static void HideLoading();
	static int32 GetLeftConConnectState();
	static int32 GetRightConConnectState();
	static void GetLeftConState(float& x, float& y, float& z, float& w, float& px, float& py, float& pz);
	static void GetRightConState(float& x, float& y, float& z, float& w, float& px, float& py, float& pz);
	static int32 GetLeftConTrigger();
	static int32 GetRightConTrigger();
	static jclass GetRawClass();
	static FString GetBuildModel();
	static bool GetDPIParameters(jobject Obj, double* DoubleArray, int32 Length);

	static int32 GetCurrentBrightness();
	static bool SetBrightness(int32 brightness);

	static int32 GetCurrentVolume();
	static int32 GetMaxVolumeNumber();
	static void SetVolume(int32 volume);
	static void VolumeUp();
	static void VolumeDown();

	static int32 GetHandness();

	static int32 Pvr_GetPsensorState();

	static FString GetDeviceSN();

	static bool IsHead6dofReset();

	static void PaymentLogin();
	static void PaymentLogout();
	static void PaymentGetUserInfo();
	static void PaymentPayWithCoin(const FString& order_number, const FString& order_title, const FString& product_detail, const FString& notify_url, const int32& pico_coin_count);
	static void PaymentPayWithPayCode(const FString& order_number, const FString& order_title, const FString& product_detail, const FString& notify_url, const FString& pay_code);
	static void PaymentQueryOrder(const FString& order_number);
public:
	static enum EGameStatus CurrentGameStatus;
};
#endif