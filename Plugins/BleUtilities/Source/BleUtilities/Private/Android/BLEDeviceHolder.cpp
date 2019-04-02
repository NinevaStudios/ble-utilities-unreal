// Copyright (c) 2019 Nineva Studios

#include "BLEDeviceHolder.h"

#if PLATFORM_ANDROID
#include "Android/AndroidApplication.h"
#include "Android/AndroidJNI.h"
#endif

static BLEDeviceHolder* pHolder = nullptr;

BLEDeviceHolder::BLEDeviceHolder()
{
}

BLEDeviceHolder* BLEDeviceHolder::GetInstance()
{
	if (!pHolder)
	{
		pHolder = new BLEDeviceHolder();
	}

	return pHolder;
}

void BLEDeviceHolder::AddDevice(FGuid guid, IBLEDeviceInterface* pDevice)
{
	BleDevices.Add(guid, pDevice);
}

void BLEDeviceHolder::RemoveDevice(FGuid guid)
{
	BleDevices.Remove(guid);
}

IBLEDeviceInterface* BLEDeviceHolder::GetDevice(FGuid guid) const
{
	if (BleDevices.Find(guid))
	{
		return BleDevices[guid];
	}

	return nullptr;
}

#if PLATFORM_ANDROID

TArray<uint8> ConvertToByteArray(jbyteArray javaArray)
{
	TArray<uint8> byteArray;
	JNIEnv* Env = FAndroidApplication::GetJavaEnv();
	jbyte* javaByte = Env->GetByteArrayElements(javaArray, 0);
	int length = Env->GetArrayLength(javaArray);
	for (int i = 0; i < length; i++)
	{
		byteArray.Add(javaByte[i]);
	}
	return byteArray;
}

JNI_METHOD void Java_com_ninevastudios_androidbleunrealapi_BleUtilitiesDevice_onNotificationReceived(JNIEnv* env, jclass clazz, jstring guid, jbyteArray data)
{
	UE_LOG(LogTemp, Warning, TEXT("BLEUTILS => Java_com_ninevastudios_androidbleunrealapi_NinevaBleDevice_onNotificationReceived"));

	const char* charsGuid = env->GetStringUTFChars(guid, 0);
	FString strGuid = FString(UTF8_TO_TCHAR(charsGuid));
	env->ReleaseStringUTFChars(guid, charsGuid);

	if (!pHolder)
	{
		return;
	}

	FGuid DeviceGuid;
	if (FGuid::Parse(strGuid, DeviceGuid))
	{
		auto Device = pHolder->GetDevice(DeviceGuid);
		if (Device)
		{
			UBLEDevice* BLEDevice = dynamic_cast<UBLEDevice*>(Device);

			TArray<uint8> bytes = ConvertToByteArray(data);

			BLEDevice->OnNotificationReceived(bytes);
		}
	}
}

JNI_METHOD void Java_com_ninevastudios_androidbleunrealapi_BleUtilitiesDevice_onUnsubscribed(JNIEnv* env, jclass clazz, jstring guid)
{
	UE_LOG(LogTemp, Warning, TEXT("BLEUTILS => Java_com_ninevastudios_androidbleunrealapi_NinevaBleDevice_onUnsubscribed"));

	const char* charsGuid = env->GetStringUTFChars(guid, 0);
	FString strGuid = FString(UTF8_TO_TCHAR(charsGuid));
	env->ReleaseStringUTFChars(guid, charsGuid);

	if (!pHolder)
	{
		return;
	}

	FGuid DeviceGuid;
	if (FGuid::Parse(strGuid, DeviceGuid))
	{
		auto Device = pHolder->GetDevice(DeviceGuid);
		if (Device)
		{
			UBLEDevice* BLEDevice = dynamic_cast<UBLEDevice*>(Device);
			BLEDevice->OnStopNotifications();
		}
	}
}

JNI_METHOD void Java_com_ninevastudios_androidbleunrealapi_BleUtilitiesDevice_onNotificationError(JNIEnv* env, jclass clazz, jstring guid, jstring error)
{
	UE_LOG(LogTemp, Warning, TEXT("BLEUTILS => Java_com_ninevastudios_androidbleunrealapi_NinevaBleDevice_onNotificationError"));

	const char* charsGuid = env->GetStringUTFChars(guid, 0);
	FString strGuid = FString(UTF8_TO_TCHAR(charsGuid));
	env->ReleaseStringUTFChars(guid, charsGuid);

	if (!pHolder)
	{
		return;
	}

	FGuid DeviceGuid;
	if (FGuid::Parse(strGuid, DeviceGuid))
	{
		auto Device = pHolder->GetDevice(DeviceGuid);
		if (Device)
		{
			UBLEDevice* BLEDevice = dynamic_cast<UBLEDevice*>(Device);

			const char* charsError = env->GetStringUTFChars(error, 0);
			FString strError = FString(UTF8_TO_TCHAR(charsError));
			env->ReleaseStringUTFChars(error, charsError);

			BLEDevice->OnNotificationError(strError);
		}
	}
}

#endif