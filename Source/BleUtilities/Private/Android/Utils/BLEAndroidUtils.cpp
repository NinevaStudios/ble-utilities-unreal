// Copyright (c) 2019 Nineva Studios

#include "BLEAndroidUtils.h"
#include "Android/AndroidJNI.h"

jobject BLEAndroidUtils::GetJavaString(FString string)
{
	JNIEnv*	JEnv = AndroidJavaEnv::GetJavaEnv();
	jstring local = JEnv->NewStringUTF(TCHAR_TO_UTF8(*string));
	jstring result = (jstring)JEnv->NewGlobalRef(local);
	JEnv->DeleteLocalRef(local);
	return result;
}

FString BLEAndroidUtils::GetFString(jstring string)
{
	FString result;
	if (string)
	{
		JNIEnv* JEnv = AndroidJavaEnv::GetJavaEnv();
		const char* chars = JEnv->GetStringUTFChars(string, 0);
		result = FString(UTF8_TO_TCHAR(chars));
		JEnv->ReleaseStringUTFChars(string, chars);
	}

	return result;
}

TArray<uint8> BLEAndroidUtils::ConvertToByteArray(jbyteArray javaArray)
{
	TArray<uint8> byteArray;
	JNIEnv* Env = FAndroidApplication::GetJavaEnv();
	jbyte* javaByte = Env->GetByteArrayElements(javaArray, 0);
	const int length = Env->GetArrayLength(javaArray);
	for (int i = 0; i < length; i++)
	{
		byteArray.Add(javaByte[i]);
	}
	return byteArray;
}

jbyteArray BLEAndroidUtils::GetJavaArray(TArray<uint8> data)
{
	const int dataLength = data.Num();
	JNIEnv* Env = FAndroidApplication::GetJavaEnv();
	jbyteArray localArray = Env->NewByteArray(dataLength);

	jbyte buf[dataLength];
	for (int i = 0; i < dataLength; i++)
	{
		buf[i] = data[i];
	}

	Env->SetByteArrayRegion(localArray, 0, dataLength, buf);
	jbyteArray resultArray = (jbyteArray)Env->NewGlobalRef(localArray);
	Env->DeleteLocalRef(localArray);

	return resultArray;
}

jobjectArray BLEAndroidUtils::GetJavaArray(TArray<FString> data)
{
	const int dataLength = data.Num();
	JNIEnv* Env = FAndroidApplication::GetJavaEnv();
	jobjectArray localArray = Env->NewObjectArray(dataLength, FJavaWrapper::JavaStringClass, 0);

	for (int i = 0; i < dataLength; i++)
	{
		jobject javaString = BLEAndroidUtils::GetJavaString(data[i]);
		Env->SetObjectArrayElement(localArray, i, javaString);
	}

	jobjectArray resultArray = (jobjectArray)Env->NewGlobalRef(localArray);
	Env->DeleteLocalRef(localArray);

	return resultArray;
}
