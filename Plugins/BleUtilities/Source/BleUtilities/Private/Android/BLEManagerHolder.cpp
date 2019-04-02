// Copyright (c) 2019 Nineva Studios

#include "BLEManagerHolder.h"
#include "BLEManager.h"

#if PLATFORM_ANDROID
#include "Android/AndroidApplication.h"
#include "Android/AndroidJNI.h"
#endif

static BLEManagerHolder* pManagerHolder = nullptr;

BLEManagerHolder::BLEManagerHolder()
{
}

BLEManagerHolder* BLEManagerHolder::GetInstance()
{
	if (!pManagerHolder)
	{
		pManagerHolder = new BLEManagerHolder();
	}

	return pManagerHolder;
}

void BLEManagerHolder::AddManager(FGuid guid, IBLEManagerInterface* pManager)
{
	BleManagers.Add(guid, pManager);
}

void BLEManagerHolder::RemoveManager(FGuid guid)
{
	BleManagers.Remove(guid);
}

IBLEManagerInterface* BLEManagerHolder::GetManager(FGuid guid) const
{
	if (BleManagers.Find(guid))
	{
		return BleManagers[guid];
	}

	return nullptr;
}

#if PLATFORM_ANDROID

JNI_METHOD void Java_com_ninevastudios_androidbleunrealapi_BleUtilitiesManager_onNewDeviceDiscovered(JNIEnv* env, jclass clazz, jstring guid, jstring deviceId)
{
	UE_LOG(LogTemp, Warning, TEXT("BLEUTILS => Java_com_ninevastudios_androidbleunrealapi_BleUtils_onNewDeviceDiscovered"));

	const char* charsGuid = env->GetStringUTFChars(guid, 0);
	FString strGuid = FString(UTF8_TO_TCHAR(charsGuid));
	env->ReleaseStringUTFChars(guid, charsGuid);

	if (!pManagerHolder)
	{
		return;
	}

	FGuid managerGuid;
	if (FGuid::Parse(strGuid, managerGuid))
	{
		auto manager = pManagerHolder->GetManager(managerGuid);
		if (manager)
		{
			UBLEManager* BLEManager = dynamic_cast<UBLEManager*>(manager);

			const char* charsDeviceId = env->GetStringUTFChars(deviceId, 0);
			FString strDeviceId = FString(UTF8_TO_TCHAR(charsDeviceId));
			env->ReleaseStringUTFChars(deviceId, charsDeviceId);

			BLEManager->OnDeviceFound(strDeviceId);
 		}
	}
}

JNI_METHOD void Java_com_ninevastudios_androidbleunrealapi_BleUtilitiesDevice_onDeviceConnected(JNIEnv* env, jclass clazz, jstring guid)
{
	UE_LOG(LogTemp, Warning, TEXT("BLEUTILS => Java_com_ninevastudios_androidbleunrealapi_BleUtils_onNewDeviceDiscovered"));

	const char* charsGuid = env->GetStringUTFChars(guid, 0);
	FString strGuid = FString(UTF8_TO_TCHAR(charsGuid));
	env->ReleaseStringUTFChars(guid, charsGuid);

	if (!pManagerHolder)
	{
		return;
	}

	FGuid managerGuid;
	if (FGuid::Parse(strGuid, managerGuid))
	{
		auto manager = pManagerHolder->GetManager(managerGuid);
		if (manager)
		{
			UBLEManager* BLEManager = dynamic_cast<UBLEManager*>(manager);
			BLEManager->OnDeviceConnected();
		}
	}
}

JNI_METHOD void Java_com_ninevastudios_androidbleunrealapi_BleUtilitiesDevice_onDeviceDisconnected(JNIEnv* env, jclass clazz, jstring guid)
{
	UE_LOG(LogTemp, Warning, TEXT("BLEUTILS => Java_com_ninevastudios_androidbleunrealapi_BleUtils_onNewDeviceDiscovered"));

	const char* charsGuid = env->GetStringUTFChars(guid, 0);
	FString strGuid = FString(UTF8_TO_TCHAR(charsGuid));
	env->ReleaseStringUTFChars(guid, charsGuid);

	if (!pManagerHolder)
	{
		return;
	}

	FGuid managerGuid;
	if (FGuid::Parse(strGuid, managerGuid))
	{
		auto manager = pManagerHolder->GetManager(managerGuid);
		if (manager)
		{
			UBLEManager* BLEManager = dynamic_cast<UBLEManager*>(manager);
			BLEManager->OnDeviceDisconnected();
		}
	}
}

#endif