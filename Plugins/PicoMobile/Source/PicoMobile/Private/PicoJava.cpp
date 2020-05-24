/*
* Copyright Pico Technology Co, Ltd. All Rights Reserved.
*/

#include "PicoJava.h"
#include "PicoImport.h"

#if PLATFORM_ANDROID

static bool AndroidThunkCpp_verifyAPP(FString appID, FString publicKey)
{
	bool ret = false;
	if (JNIEnv* Env = FAndroidApplication::GetJavaEnv())
	{

		jstring j_appID = Env->NewStringUTF(TCHAR_TO_UTF8(*appID));
		jstring j_publicKey = Env->NewStringUTF(TCHAR_TO_UTF8(*publicKey));
		static jmethodID Method = FJavaWrapper::FindMethod(Env, FJavaWrapper::GameActivityClassID, "AndroidThunkJava_verifyAPP", "(Ljava/lang/String;Ljava/lang/String;)Z", false);
		ret = (bool)FJavaWrapper::CallBooleanMethod(Env, FJavaWrapper::GameActivityThis, Method, j_appID, j_publicKey);
		Env->DeleteLocalRef(j_appID);
		Env->DeleteLocalRef(j_publicKey);
	}
	return ret;
}

static inline void AndroidThunkCpp_HideLoading()
{
	if (JNIEnv* Env = FAndroidApplication::GetJavaEnv())
	{
		static jmethodID Method = FJavaWrapper::FindMethod(Env, FJavaWrapper::GameActivityClassID, "AndroidThunkJava_HideLoading", "()V", false);
		FJavaWrapper::CallVoidMethod(Env, FJavaWrapper::GameActivityThis, Method);
	}
}

static int32 GetLeftControllerConnectState()
{
	int32 connectState = 0;
	if (JNIEnv* Env = FAndroidApplication::GetJavaEnv())
	{
		static jmethodID Method = FJavaWrapper::FindMethod(Env, FJavaWrapper::GameActivityClassID, "GetMainControllerConnectState", "()I", false);
		connectState = (int32)FJavaWrapper::CallIntMethod(Env, FJavaWrapper::GameActivityThis, Method);
	}
	return connectState;
}

static int32 GetRightControllerConnectState()
{
	int32 connectState = 0;
	if (JNIEnv* Env = FAndroidApplication::GetJavaEnv())
	{
		static jmethodID Method = FJavaWrapper::FindMethod(Env, FJavaWrapper::GameActivityClassID, "GetSubControllerConnectState", "()I", false);
		connectState = (int32)FJavaWrapper::CallIntMethod(Env, FJavaWrapper::GameActivityThis, Method);
	}
	return connectState;
}

static void GetLeftControllerFixedSensorState(float& x, float& y, float& z, float& w, float& px, float& py, float& pz)
{
	if (JNIEnv* Env = FAndroidApplication::GetJavaEnv())
	{
		static jmethodID Method = FJavaWrapper::FindMethod(Env, FJavaWrapper::GameActivityClassID, "getLeftControllerFixedSensorState", "()[F", false);
		jfloatArray jfArray = (jfloatArray)FJavaWrapper::CallObjectMethod(Env, FJavaWrapper::GameActivityThis, Method);
		jfloat *jfp = Env->GetFloatArrayElements(jfArray, JNI_FALSE);
		x = jfp[0];
		y = jfp[1];
		z = jfp[2];
		w = jfp[3];
		px = jfp[4];
		py = jfp[5];
		pz = jfp[6];
		Env->ReleaseFloatArrayElements(jfArray, jfp, JNI_ABORT);
		Env->DeleteLocalRef(jfArray);
	}
}

static void GetRightControllerFixedSensorState(float& x, float& y, float& z, float& w, float& px, float& py, float& pz)
{
	if (JNIEnv* Env = FAndroidApplication::GetJavaEnv())
	{
		static jmethodID Method = FJavaWrapper::FindMethod(Env, FJavaWrapper::GameActivityClassID, "getRightControllerFixedSensorState", "()[F", false);
		jfloatArray jfArray = (jfloatArray)FJavaWrapper::CallObjectMethod(Env, FJavaWrapper::GameActivityThis, Method);
		jfloat *jfp = Env->GetFloatArrayElements(jfArray, JNI_FALSE);
		x = jfp[0];
		y = jfp[1];
		z = jfp[2];
		w = jfp[3];
		px = jfp[4];
		py = jfp[5];
		pz = jfp[6];
		Env->ReleaseFloatArrayElements(jfArray, jfp, JNI_ABORT);
		Env->DeleteLocalRef(jfArray);
	}
}

static int32 GetLeftControllerTrigger()
{
	int32 TriggerPress = -1;
	if (JNIEnv* Env = FAndroidApplication::GetJavaEnv())
	{
		static jmethodID Method = FJavaWrapper::FindMethod(Env, FJavaWrapper::GameActivityClassID, "getMainControllerTriggerNum", "()I", false);
		TriggerPress = (int32)FJavaWrapper::CallIntMethod(Env, FJavaWrapper::GameActivityThis, Method);
	}
	if (TriggerPress <= 0)
	{
		return 0;
	}
	else
	{
		return 8;
	}
}

static int32 GetRightControllerTrigger()
{
	int32 TriggerPress = -1;
	if (JNIEnv* Env = FAndroidApplication::GetJavaEnv())
	{
		static jmethodID Method = FJavaWrapper::FindMethod(Env, FJavaWrapper::GameActivityClassID, "getSubControllerTriggerNum", "()I", false);
		TriggerPress = (int32)FJavaWrapper::CallIntMethod(Env, FJavaWrapper::GameActivityThis, Method);
	}
	if (TriggerPress <= 0)
	{
		return 0;
	}
	else
	{
		return 8;
	}
}

static inline jclass AndroidThunkCpp_GetRawClass()
{
	jclass ret = nullptr;
	if (JNIEnv* Env = FAndroidApplication::GetJavaEnv())
	{
		static jmethodID Method = FJavaWrapper::FindMethod(Env, FJavaWrapper::GameActivityClassID, "AndroidThunkJava_UEPvr_GetRawClass", "()Lcom/psmart/vrlib/VrActivity;", false);
		ret = (jclass)FJavaWrapper::CallObjectMethod(Env, FJavaWrapper::GameActivityThis, Method);
	}
	return ret;
}

static inline FString AndroidThunkCpp_GetBuildModel()
{
	FString ret = FString("");
	if (JNIEnv* Env = FAndroidApplication::GetJavaEnv())
	{
		static jmethodID Method = FJavaWrapper::FindMethod(Env, FJavaWrapper::GameActivityClassID, "AndroidThunkJava_UEPvr_GetBuildModel", "()Ljava/lang/String;", false);
		jstring JavaString = (jstring)FJavaWrapper::CallObjectMethod(Env, FJavaWrapper::GameActivityThis, Method);
		if (JavaString != NULL)
		{
			const char* JavaChars = Env->GetStringUTFChars(JavaString, 0);
			ret = FString(UTF8_TO_TCHAR(JavaChars));
			Env->ReleaseStringUTFChars(JavaString, JavaChars);
			Env->DeleteLocalRef(JavaString);
		}
	}
	return ret;
}

static inline bool AndroidThunkCpp_GetDPIParameters(jobject obj , double* double_array , int32 length)
{
	bool IsSuccess = false;
	jdoubleArray J_array = 0;
	if (JNIEnv* Env = FAndroidApplication::GetJavaEnv())
	{
		static jmethodID Method = FJavaWrapper::FindMethod(Env, FJavaWrapper::GameActivityClassID, "AndroidThunkJava_UEPvr_GetDPIParameters", "(Landroid/content/Context;)[D", false);
		J_array = (jdoubleArray)FJavaWrapper::CallObjectMethod(Env, FJavaWrapper::GameActivityThis, Method, obj);
		
		jsize J_array_size = Env->GetArrayLength(J_array);
		if (length == J_array_size)
		{
			double* FirstArrayElement = Env->GetDoubleArrayElements(J_array, nullptr);

			FMemory::Memcpy(double_array, FirstArrayElement, length * sizeof(double));
			IsSuccess = true;
		}

	}
	return IsSuccess;
}

static inline int32 AndroidThunkCpp_GetCurrentBrightness()
{
	int32 currentBrightness = -1;
	if (JNIEnv* Env = FAndroidApplication::GetJavaEnv())
	{
		static jmethodID Method = FJavaWrapper::FindMethod(Env, FJavaWrapper::GameActivityClassID, "AndroidThunkJava_UEPvr_GetCurrentBrightness", "()I", false);
		currentBrightness = (int32)FJavaWrapper::CallIntMethod(Env, FJavaWrapper::GameActivityThis, Method);
	}
	return currentBrightness;
}

static inline bool AndroidThunkCpp_SetBrightness(int32 brightness)
{
	bool ret = false;
	if (JNIEnv* Env = FAndroidApplication::GetJavaEnv())
	{
		static jmethodID Method = FJavaWrapper::FindMethod(Env, FJavaWrapper::GameActivityClassID, "AndroidThunkJava_UEPvr_SetBrightness", "(I)Z", false);
		ret = (bool)FJavaWrapper::CallBooleanMethod(Env, FJavaWrapper::GameActivityThis, Method,brightness);
	}
	return ret;
}

static inline int32 AndroidThunkCpp_GetCurrentVolume()
{
	int32 currentVolume = -1;
	if (JNIEnv* Env = FAndroidApplication::GetJavaEnv())
	{
		static jmethodID Method = FJavaWrapper::FindMethod(Env, FJavaWrapper::GameActivityClassID, "AndroidThunkJava_UEPvr_GetCurrentVolume", "()I", false);
		currentVolume = (int32)FJavaWrapper::CallIntMethod(Env, FJavaWrapper::GameActivityThis, Method);
	}
	return currentVolume;
}

static inline int32 AndroidThunkCpp_GetMaxVolumeNumber()
{
	int32 maxVolume = -1;
	if (JNIEnv* Env = FAndroidApplication::GetJavaEnv())
	{
		static jmethodID Method = FJavaWrapper::FindMethod(Env, FJavaWrapper::GameActivityClassID, "AndroidThunkJava_UEPvr_GetMaxVolumeNumber", "()I", false);
		maxVolume = (int32)FJavaWrapper::CallIntMethod(Env, FJavaWrapper::GameActivityThis, Method);
	}
	return maxVolume;
}

static inline void AndroidThunkCpp_SetVolume(int32 volume)
{
	if (JNIEnv* Env = FAndroidApplication::GetJavaEnv())
	{
		static jmethodID Method = FJavaWrapper::FindMethod(Env, FJavaWrapper::GameActivityClassID, "AndroidThunkJava_UEPvr_SetVolume", "(I)V", false);
		FJavaWrapper::CallVoidMethod(Env, FJavaWrapper::GameActivityThis, Method, volume);
	}
}

static inline void AndroidThunkCpp_VolumeUp()
{
	if (JNIEnv* Env = FAndroidApplication::GetJavaEnv())
	{
		static jmethodID Method = FJavaWrapper::FindMethod(Env, FJavaWrapper::GameActivityClassID, "AndroidThunkJava_UEPvr_VolumeUp", "()V", false);
		FJavaWrapper::CallVoidMethod(Env, FJavaWrapper::GameActivityThis, Method);
	}
}

static inline void AndroidThunkCpp_VolumeDown()
{
	if (JNIEnv* Env = FAndroidApplication::GetJavaEnv())
	{
		static jmethodID Method = FJavaWrapper::FindMethod(Env, FJavaWrapper::GameActivityClassID, "AndroidThunkJava_UEPvr_VolumeDown", "()V", false);
		FJavaWrapper::CallVoidMethod(Env, FJavaWrapper::GameActivityThis, Method);
	}
}

static inline int32 AndroidThunkCpp_GetHandness()
{
	int32 handness = 0;
	if (JNIEnv* Env = FAndroidApplication::GetJavaEnv())
	{
		static jmethodID Method = FJavaWrapper::FindMethod(Env, FJavaWrapper::GameActivityClassID, "AndroidThunkJava_UEPvr_GetHandness", "()I", false);
		handness = (int32)FJavaWrapper::CallIntMethod(Env, FJavaWrapper::GameActivityThis, Method);
	}
	return handness;
}

static inline void AndroidThunkCpp_Payment_Login()
{
	if (JNIEnv* Env = FAndroidApplication::GetJavaEnv())
	{
		static jmethodID Method = FJavaWrapper::FindMethod(Env, FJavaWrapper::GameActivityClassID, "AndroidThunkJava_UEPvr_Payment_Login", "()V", false);
		FJavaWrapper::CallVoidMethod(Env, FJavaWrapper::GameActivityThis, Method);
	}
}

static inline void AndroidThunkCpp_Payment_Logout()
{
	if (JNIEnv* Env = FAndroidApplication::GetJavaEnv())
	{
		static jmethodID Method = FJavaWrapper::FindMethod(Env, FJavaWrapper::GameActivityClassID, "AndroidThunkJava_UEPvr_Payment_Logout", "()V", false);
		FJavaWrapper::CallVoidMethod(Env, FJavaWrapper::GameActivityThis, Method);
	}
}

static inline void AndroidThunkCpp_Payment_GetUserInfo()
{
	if (JNIEnv* Env = FAndroidApplication::GetJavaEnv())
	{
		static jmethodID Method = FJavaWrapper::FindMethod(Env, FJavaWrapper::GameActivityClassID, "AndroidThunkJava_UEPvr_Payment_GetUserInfo", "()V", false);
		FJavaWrapper::CallVoidMethod(Env, FJavaWrapper::GameActivityThis, Method);
	}
}

static inline void AndroidThunkCpp_Payment_PayWithCoin(const FString& order_number, const FString& order_title, const FString& product_detail, const FString& notify_url, const int32& pico_coin_count)
{
	if (JNIEnv* Env = FAndroidApplication::GetJavaEnv())
	{
		jstring j_order_number = Env->NewStringUTF(TCHAR_TO_UTF8(*order_number));
		jstring j_order_title = Env->NewStringUTF(TCHAR_TO_UTF8(*order_title));
		jstring j_product_detail = Env->NewStringUTF(TCHAR_TO_UTF8(*product_detail));
		jstring j_notify_url = Env->NewStringUTF(TCHAR_TO_UTF8(*notify_url));
		int32 j_coin_count = pico_coin_count;

		static jmethodID Method = FJavaWrapper::FindMethod(Env, FJavaWrapper::GameActivityClassID, "AndroidThunkJava_UEPvr_Payment_PayWithCoin", "(Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;I)V", false);
		FJavaWrapper::CallVoidMethod(Env, FJavaWrapper::GameActivityThis, Method, j_order_number, j_order_title, j_product_detail, j_notify_url, j_coin_count);
		
		Env->DeleteLocalRef(j_order_number);
		Env->DeleteLocalRef(j_order_title);
		Env->DeleteLocalRef(j_product_detail);
		Env->DeleteLocalRef(j_notify_url);
	}
}

static inline void AndroidThunkCpp_Payment_PayWithPayCode(const FString& order_number, const FString& order_title, const FString& product_detail, const FString& notify_url, const FString& pay_code)
{
	if (JNIEnv* Env = FAndroidApplication::GetJavaEnv())
	{
		jstring j_order_number = Env->NewStringUTF(TCHAR_TO_UTF8(*order_number));
		jstring j_order_title = Env->NewStringUTF(TCHAR_TO_UTF8(*order_title));
		jstring j_product_detail = Env->NewStringUTF(TCHAR_TO_UTF8(*product_detail));
		jstring j_notify_url = Env->NewStringUTF(TCHAR_TO_UTF8(*notify_url));
		jstring j_pay_code = Env->NewStringUTF(TCHAR_TO_UTF8(*pay_code));

		static jmethodID Method = FJavaWrapper::FindMethod(Env, FJavaWrapper::GameActivityClassID, "AndroidThunkJava_UEPvr_Payment_PayWithPayCode", "(Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;)V", false);
		FJavaWrapper::CallVoidMethod(Env, FJavaWrapper::GameActivityThis, Method, j_order_number, j_order_title, j_product_detail, j_notify_url, j_pay_code);

		Env->DeleteLocalRef(j_order_number);
		Env->DeleteLocalRef(j_order_title);
		Env->DeleteLocalRef(j_product_detail);
		Env->DeleteLocalRef(j_notify_url);
		Env->DeleteLocalRef(j_pay_code);
	}
}

static inline void AndroidThunkCpp_Payment_QueryOrder(const FString& order_number)
{
	if (JNIEnv* Env = FAndroidApplication::GetJavaEnv())
	{
		jstring j_order_number = Env->NewStringUTF(TCHAR_TO_UTF8(*order_number));

		static jmethodID Method = FJavaWrapper::FindMethod(Env, FJavaWrapper::GameActivityClassID, "AndroidThunkJava_UEPvr_Payment_QueryOrder", "(Ljava/lang/String;)V", false);
		FJavaWrapper::CallVoidMethod(Env, FJavaWrapper::GameActivityThis, Method, j_order_number);

		Env->DeleteLocalRef(j_order_number);
	}
}

static inline int32 AndroidThunkCpp_GetPsensorState()
{
	int32 handness = 0;
	if (JNIEnv* Env = FAndroidApplication::GetJavaEnv())
	{
		static jmethodID Method = FJavaWrapper::FindMethod(Env, FJavaWrapper::GameActivityClassID, "AndroidThunkJava_UEPvr_GetPsensorState", "()I", false);
		handness = (int32)FJavaWrapper::CallIntMethod(Env, FJavaWrapper::GameActivityThis, Method);
	}
	return handness;
}

static inline FString AndroidThunkCpp_GetDeviceSN()
{
	FString ret = FString("");
	if (JNIEnv* Env = FAndroidApplication::GetJavaEnv())
	{
		static jmethodID Method = FJavaWrapper::FindMethod(Env, FJavaWrapper::GameActivityClassID, "AndroidThunkJava_UEPvr_GetDeviceSN", "()Ljava/lang/String;", false);
		jstring JavaString = (jstring)FJavaWrapper::CallObjectMethod(Env, FJavaWrapper::GameActivityThis, Method);
		if (JavaString != NULL)
		{
			const char* JavaChars = Env->GetStringUTFChars(JavaString, 0);
			ret = FString(UTF8_TO_TCHAR(JavaChars));
			Env->ReleaseStringUTFChars(JavaString, JavaChars);
			Env->DeleteLocalRef(JavaString);
		}
	}
	return ret;
}

//============================================================

enum EGameStatus FPicoJava::CurrentGameStatus = NoValue;

FPicoJava::FPicoJava()
{
}

FPicoJava::~FPicoJava()
{
}

bool FPicoJava::VerifyAPP(FString appID, FString publicKey)
{
	return AndroidThunkCpp_verifyAPP(appID, publicKey);
}

void FPicoJava::HideLoading()
{
	AndroidThunkCpp_HideLoading();
}

int32 FPicoJava::GetLeftConConnectState()
{
	return GetLeftControllerConnectState();
}

int32 FPicoJava::GetRightConConnectState()
{
	return GetRightControllerConnectState();
}

void FPicoJava::GetLeftConState(float& x, float& y, float& z, float& w, float& px, float& py, float& pz)
{
	GetLeftControllerFixedSensorState(x, y, z, w, px, py, pz);
}

void FPicoJava::GetRightConState(float& x, float& y, float& z, float& w, float& px, float& py, float& pz)
{
	GetRightControllerFixedSensorState(x, y, z, w, px, py, pz);
}

int32 FPicoJava::GetLeftConTrigger()
{
	return GetLeftControllerTrigger();
}

int32 FPicoJava::GetRightConTrigger()
{
	return GetRightControllerTrigger();
}

jclass FPicoJava::GetRawClass()
{
	return AndroidThunkCpp_GetRawClass();
}

FString FPicoJava::GetBuildModel()
{
	return AndroidThunkCpp_GetBuildModel();
}

bool FPicoJava::GetDPIParameters(jobject Obj, double* DoubleArray, int32 Length)
{
	return AndroidThunkCpp_GetDPIParameters(Obj, DoubleArray, Length);
}

int32 FPicoJava::GetCurrentBrightness()
{
	return AndroidThunkCpp_GetCurrentBrightness();
}

bool FPicoJava::SetBrightness(int32 brightness)
{
	return AndroidThunkCpp_SetBrightness(brightness);
}

int32 FPicoJava::GetCurrentVolume()
{
	return AndroidThunkCpp_GetCurrentVolume();
}

int32 FPicoJava::GetMaxVolumeNumber()
{
	return AndroidThunkCpp_GetMaxVolumeNumber();
}

void FPicoJava::SetVolume(int32 volume)
{
	AndroidThunkCpp_SetVolume(volume);
}

void FPicoJava::VolumeUp()
{
	AndroidThunkCpp_VolumeUp();
}

void FPicoJava::VolumeDown()
{
	AndroidThunkCpp_VolumeDown();
}

int32 FPicoJava::GetHandness()
{
	return AndroidThunkCpp_GetHandness();
}

int32 FPicoJava::Pvr_GetPsensorState()
{
	return AndroidThunkCpp_GetPsensorState();
}

FString FPicoJava::GetDeviceSN()
{
	return AndroidThunkCpp_GetDeviceSN();
}

bool FPicoJava::IsHead6dofReset()
{
	JNIEnv *Env = FAndroidApplication::GetJavaEnv();

	jmethodID id_AndroidThunkJava_UEPvr_IsHead6dofReset = FJavaWrapper::FindMethod(Env,
		FJavaWrapper::GameActivityClassID,
		"AndroidThunkJava_UEPvr_IsHead6dofReset",
		"()Z", false);

	return FJavaWrapper::CallBooleanMethod(
		Env,
		FJavaWrapper::GameActivityThis,
		id_AndroidThunkJava_UEPvr_IsHead6dofReset);
}

void FPicoJava::PaymentLogin()
{
	AndroidThunkCpp_Payment_Login();
}

void FPicoJava::PaymentLogout()
{
	AndroidThunkCpp_Payment_Logout();
}

void FPicoJava::PaymentGetUserInfo()
{
	AndroidThunkCpp_Payment_GetUserInfo();
}

void FPicoJava::PaymentPayWithCoin(const FString& order_number, const FString& order_title, const FString& product_detail, const FString& notify_url, const int32& pico_coin_count)
{
	AndroidThunkCpp_Payment_PayWithCoin(order_number, order_title, product_detail, notify_url,pico_coin_count);
}

void FPicoJava::PaymentPayWithPayCode(const FString& order_number, const FString& order_title, const FString& product_detail, const FString& notify_url, const FString& pay_code)
{
	AndroidThunkCpp_Payment_PayWithPayCode(order_number, order_title, product_detail, notify_url, pay_code);
}

void FPicoJava::PaymentQueryOrder(const FString& order_number)
{
	AndroidThunkCpp_Payment_QueryOrder(order_number);
}

#endif

#if PLATFORM_ANDROID
extern "C" JNIEXPORT void JNICALL Java_com_psmart_vrlib_HomeKeyReceiverNative_nativePicoHomeReset(JNIEnv * jni, jclass clazz)
{
	PicoSDK::Sensor::OptionalResetSensor(PicoSDK::Sensor::GMainSensorIndex, 1, 1);
}
#endif