// Copyright (c) 2019 Nineva Studios

#include "BLEDeviceHolder.h"

#include "Async/Async.h"
#include "BLEDevice.h"
#include "BleLog.h"
#include "Utils/BLEAndroidUtils.h"

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

void BLEDeviceHolder::AddDevice(DeviceId id, IBLEDeviceInterface* pDevice)
{
	UE_LOG(LogBle, Verbose, TEXT("Add device %s with service %s"), *id.Key, *id.Value);

	if (BleDevices.Contains(id))
	{
		UE_LOG(LogBle, Warning, TEXT("Device %s with service %s already exists"), *id.Key, *id.Value);
		return;
	}

	BleDevices.Add(id, pDevice);
}

void BLEDeviceHolder::RemoveDevice(DeviceId id)
{
	UE_LOG(LogBle, Verbose, TEXT("Remove device %s with service %s"), *id.Key, *id.Value);
	BleDevices.Remove(id);
}

IBLEDeviceInterface* const BLEDeviceHolder::GetDevice(DeviceId id) const
{
	auto* device = BleDevices.Find(id);
	return device ? *device : nullptr;
}

#if PLATFORM_ANDROID

JNI_METHOD void Java_com_ninevastudios_androidbleunrealapi_BleUtilitiesDevice_onReadSuccess(JNIEnv* env, jclass clazz, 
	jstring jdeviceUUID, jstring jserviceUUID, jstring jcharacteristicUUID, jbyteArray data)
{
	UE_LOG(LogBle, Verbose, TEXT("Java callback onReadSuccess"));

	if (!pHolder)
	{
		return;
	}

	FString deviceUUID = BLEAndroidUtils::GetFString(jdeviceUUID);
	FString serviceUUID = BLEAndroidUtils::GetFString(jserviceUUID);
	FString characteristicUUID = BLEAndroidUtils::GetFString(jcharacteristicUUID);

	UE_LOG(LogBle, Verbose, TEXT("Device: %s\n\tService: %s\n\tCharacteristic: %s"),
		*deviceUUID, *serviceUUID, *characteristicUUID);

	auto Device = pHolder->GetDevice(MakeTuple(deviceUUID.ToLower(), serviceUUID.ToLower()));
	if (Device)
	{
		UBLEDevice* BLEDevice = dynamic_cast<UBLEDevice*>(Device);

		TArray<uint8> bytes = BLEAndroidUtils::ConvertToByteArray(data);

		AsyncTask(ENamedThreads::GameThread, [=]() {
			BLEDevice->OnReadSuccess(characteristicUUID.ToLower(), bytes);
		});
	}
}

JNI_METHOD void Java_com_ninevastudios_androidbleunrealapi_BleUtilitiesDevice_onReadError(JNIEnv* env, jclass clazz, 
	jstring jdeviceUUID, jstring jserviceUUID, jstring jcharacteristicUUID, jstring error)
{
	UE_LOG(LogBle, Verbose, TEXT("Java callback onReadError"));

	if (!pHolder)
	{
		return;
	}

	FString deviceUUID = BLEAndroidUtils::GetFString(jdeviceUUID);
	FString serviceUUID = BLEAndroidUtils::GetFString(jserviceUUID);
	FString characteristicUUID = BLEAndroidUtils::GetFString(jcharacteristicUUID);
	FString errorMessage = BLEAndroidUtils::GetFString(error);

	UE_LOG(LogBle, Verbose, TEXT("Device: %s\n\tService: %s\n\tCharacteristic: %s"), 
		*deviceUUID, *serviceUUID, *characteristicUUID);
	UE_LOG(LogBle, Error, TEXT("Read Error: %s"), *errorMessage);

	auto Device = pHolder->GetDevice(MakeTuple(deviceUUID.ToLower(), serviceUUID.ToLower()));
	if (Device)
	{
		UBLEDevice* BLEDevice = dynamic_cast<UBLEDevice*>(Device);

		AsyncTask(ENamedThreads::GameThread, [=]() {
			BLEDevice->OnReadError(characteristicUUID.ToLower(), errorMessage);
		});
	}
}

JNI_METHOD void Java_com_ninevastudios_androidbleunrealapi_BleUtilitiesDevice_onWriteSuccess(JNIEnv* env, jclass clazz, 
	jstring jdeviceUUID, jstring jcharacteristicUUID, jstring jserviceUUID)
{
	UE_LOG(LogBle, Verbose, TEXT("Java callback onWriteSuccess"));

	if (!pHolder)
	{
		return;
	}

	FString deviceUUID = BLEAndroidUtils::GetFString(jdeviceUUID);
	FString serviceUUID = BLEAndroidUtils::GetFString(jserviceUUID);
	FString characteristicUUID = BLEAndroidUtils::GetFString(jcharacteristicUUID);

	UE_LOG(LogBle, Verbose, TEXT("Device: %s\n\tService: %s\n\tCharacteristic: %s"),
		*deviceUUID, *serviceUUID, *characteristicUUID);

	auto Device = pHolder->GetDevice(MakeTuple(deviceUUID.ToLower(), serviceUUID.ToLower()));
	if (Device)
	{
		UBLEDevice* BLEDevice = dynamic_cast<UBLEDevice*>(Device);

		AsyncTask(ENamedThreads::GameThread, [=]() {
			BLEDevice->OnWriteSuccess(characteristicUUID.ToLower());
		});
	}
}

JNI_METHOD void Java_com_ninevastudios_androidbleunrealapi_BleUtilitiesDevice_onWriteError(JNIEnv* env, jclass clazz, 
	jstring jdeviceUUID, jstring jserviceUUID, jstring jcharacteristicUUID, jstring error)
{
	UE_LOG(LogBle, Verbose, TEXT("Java callback onWriteError"));

	if (!pHolder)
	{
		return;
	}

	FString deviceUUID = BLEAndroidUtils::GetFString(jdeviceUUID);
	FString serviceUUID = BLEAndroidUtils::GetFString(jserviceUUID);
	FString characteristicUUID = BLEAndroidUtils::GetFString(jcharacteristicUUID);
	FString errorMessage = BLEAndroidUtils::GetFString(error);

	UE_LOG(LogBle, Verbose, TEXT("Device: %s\n\tService: %s\n\tCharacteristic: %s"),
		*deviceUUID, *serviceUUID, *characteristicUUID);
	UE_LOG(LogBle, Error, TEXT("Write Error: %s"), *errorMessage);

	auto Device = pHolder->GetDevice(MakeTuple(deviceUUID.ToLower(), serviceUUID.ToLower()));
	if (Device)
	{
		UBLEDevice* BLEDevice = dynamic_cast<UBLEDevice*>(Device);

		AsyncTask(ENamedThreads::GameThread, [=]() {
			BLEDevice->OnWriteError(characteristicUUID.ToLower(), errorMessage);
		});
	}
}

JNI_METHOD void Java_com_ninevastudios_androidbleunrealapi_BleUtilitiesDevice_onNotificationReceived(JNIEnv* env, jclass clazz, 
	jstring jdeviceUUID, jstring jserviceUUID, jstring jcharacteristicUUID, jbyteArray data)
{
	UE_LOG(LogBle, Verbose, TEXT("Java callback onNotificationReceived"));
	
	if (!pHolder)
	{
		return;
	}

	FString deviceUUID = BLEAndroidUtils::GetFString(jdeviceUUID);
	FString serviceUUID = BLEAndroidUtils::GetFString(jserviceUUID);
	FString characteristicUUID = BLEAndroidUtils::GetFString(jcharacteristicUUID);

	UE_LOG(LogBle, Verbose, TEXT("BLEUTILS => onNotificationReceived -> Device: %s\n\tService: %s\n\tCharacteristic: %s"),
		*deviceUUID, *serviceUUID, *characteristicUUID);

	auto Device = pHolder->GetDevice(MakeTuple(deviceUUID.ToLower(), serviceUUID.ToLower()));
	if (Device)
	{
		UBLEDevice* BLEDevice = dynamic_cast<UBLEDevice*>(Device);

		TArray<uint8> bytes = BLEAndroidUtils::ConvertToByteArray(data);

		AsyncTask(ENamedThreads::GameThread, [=]() {
			BLEDevice->OnNotificationReceived(characteristicUUID.ToLower(), bytes);
		});	
	}
}

JNI_METHOD void Java_com_ninevastudios_androidbleunrealapi_BleUtilitiesDevice_onNotificationError(JNIEnv* env, jclass clazz,
	jstring jdeviceUUID, jstring jserviceUUID, jstring jcharacteristicUUID, jstring error)
{
	UE_LOG(LogBle, Verbose, TEXT("Java callback onNotificationError"));

	if (!pHolder)
	{
		return;
	}

	FString deviceUUID = BLEAndroidUtils::GetFString(jdeviceUUID);
	FString serviceUUID = BLEAndroidUtils::GetFString(jserviceUUID);
	FString characteristicUUID = BLEAndroidUtils::GetFString(jcharacteristicUUID);
	FString errorMessage = BLEAndroidUtils::GetFString(error);

	UE_LOG(LogBle, Verbose, TEXT("Device: %s\n\tService: %s\n\tCharacteristic: %s"),
		*deviceUUID, *serviceUUID, *characteristicUUID);
	UE_LOG(LogBle, Error, TEXT("Subscribe Error: %s"), *errorMessage);

	auto Device = pHolder->GetDevice(MakeTuple(deviceUUID.ToLower(), serviceUUID.ToLower()));
	if (Device)
	{
		UBLEDevice* BLEDevice = dynamic_cast<UBLEDevice*>(Device);

		AsyncTask(ENamedThreads::GameThread, [=]() {
			BLEDevice->OnNotificationError(characteristicUUID.ToLower(), errorMessage);
		});
	}
}

JNI_METHOD void Java_com_ninevastudios_androidbleunrealapi_BleUtilitiesDevice_onUnsubscribed(JNIEnv* env, jclass clazz, 
	jstring jdeviceUUID, jstring jserviceUUID, jstring jcharacteristicUUID)
{
	UE_LOG(LogBle, Verbose, TEXT("Java callback onUnsubscribed"));

	if (!pHolder)
	{
		return;
	}

	FString deviceUUID = BLEAndroidUtils::GetFString(jdeviceUUID);
	FString serviceUUID = BLEAndroidUtils::GetFString(jserviceUUID);
	FString characteristicUUID = BLEAndroidUtils::GetFString(jcharacteristicUUID);

	UE_LOG(LogBle, Verbose, TEXT("Device: %s\n\tService: %s\n\tCharacteristic: %s"),
		*deviceUUID, *serviceUUID, *characteristicUUID);

	auto Device = pHolder->GetDevice(MakeTuple(deviceUUID.ToLower(), serviceUUID.ToLower()));
	if (Device)
	{
		UBLEDevice* BLEDevice = dynamic_cast<UBLEDevice*>(Device);
		
		AsyncTask(ENamedThreads::GameThread, [=]() {
			BLEDevice->OnStopNotifications(characteristicUUID.ToLower());
		});
	}
}

JNI_METHOD void Java_com_ninevastudios_androidbleunrealapi_BleUtilitiesDevice_onUnsubscribedError(JNIEnv* env, jclass clazz, 
	jstring jdeviceUUID, jstring jserviceUUID, jstring jcharacteristicUUID, jstring error)
{
	UE_LOG(LogBle, Verbose, TEXT("Java callback onUnsubscribedError"));

	if (!pHolder)
	{
		return;
	}

	FString deviceUUID = BLEAndroidUtils::GetFString(jdeviceUUID);
	FString serviceUUID = BLEAndroidUtils::GetFString(jserviceUUID);
	FString characteristicUUID = BLEAndroidUtils::GetFString(jcharacteristicUUID);
	FString errorMessage = BLEAndroidUtils::GetFString(error);

	UE_LOG(LogBle, Verbose, TEXT("Device: %s\n\tService: %s\n\tCharacteristic: %s"), 
		*deviceUUID, *serviceUUID, *characteristicUUID);
	UE_LOG(LogBle, Error, TEXT("Unsubscribe Error: %s"), *errorMessage);

	auto Device = pHolder->GetDevice(MakeTuple(deviceUUID.ToLower(), serviceUUID.ToLower()));
	if (Device)
	{
		UBLEDevice* BLEDevice = dynamic_cast<UBLEDevice*>(Device);

		AsyncTask(ENamedThreads::GameThread, [=]() {
			BLEDevice->OnNotificationStopError(characteristicUUID.ToLower(), errorMessage);
		});
	}
}

#endif