// Copyright (c) 2019 Nineva Studios

#include "BLEDevice.h"

#include "BLEDeviceHolder.h"
#include "BLEManager.h"
#include "BLEManagerHolder.h"
#include "BleLog.h"
#include "Utils/BLEAndroidUtils.h"

UBLEDevice::UBLEDevice(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer) 
{
	UE_LOG(LogBle, Verbose, TEXT("UBLEDevice::UBLEDevice()"));

	if (UBLEDevice::StaticClass()->GetDefaultObject() != this)
	{
		JNIEnv* Env = FAndroidApplication::GetJavaEnv();

		jclass deviceJavaClass = FAndroidApplication::FindJavaClass("com/ninevastudios/androidbleunrealapi/BleUtilitiesDevice");

		IsDeviceConnected = FJavaWrapper::FindMethod(Env, deviceJavaClass, "isConnected", "()Z", false);
		GetDeviceNameMethod = FJavaWrapper::FindMethod(Env, deviceJavaClass, "getDeviceName", "()Ljava/lang/String;", false);
		GetDeviceIDMethod = FJavaWrapper::FindMethod(Env, deviceJavaClass, "getDeviceId", "()Ljava/lang/String;", false);
		ReadCharacteristicMethod = FJavaWrapper::FindMethod(Env, deviceJavaClass, "readValue", "(Ljava/lang/String;)V", false);
		WriteCharacteristicMethod = FJavaWrapper::FindMethod(Env, deviceJavaClass, "writeValue", "(Ljava/lang/String;[B)V", false);
		SubscribeForNotificationsMethod = FJavaWrapper::FindMethod(Env, deviceJavaClass, "subscribe", "(Ljava/lang/String;Z)V", false);
		StopNotificationsMethod = FJavaWrapper::FindMethod(Env, deviceJavaClass, "unsubscribe", "(Ljava/lang/String;)V", false);
	}	
}

void UBLEDevice::PostInitProperties()
{
	Super::PostInitProperties();

	if (UBLEDevice::StaticClass()->GetDefaultObject() != this)
	{
		// add BLE manager to root to avoid GC issues
		AddToRoot();		
	}
}

void UBLEDevice::BeginDestroy()
{
	UE_LOG(LogBle, Verbose, TEXT("UBLEManager::BeginDestroy()"));

	if (UBLEDevice::StaticClass()->GetDefaultObject() != this)
	{
		BLEDeviceHolder::GetInstance()->RemoveDevice(MakeTuple(DeviceId.ToLower(), ServiceUUID.ToLower()));

		JNIEnv * Env = FAndroidApplication::GetJavaEnv();

		Env->DeleteGlobalRef(javaDevice);

		// remove BLE manager from root so now it can be safely destroyed by GC
		RemoveFromRoot();
	}

	Super::BeginDestroy();
}

void UBLEDevice::SetDeviceInternal(jobject jDevice, FString serviceUUID)
{
	JNIEnv * Env = FAndroidApplication::GetJavaEnv();
	
	javaDevice = Env->NewGlobalRef(jDevice);
	ServiceUUID = serviceUUID;
	DeviceId = GetDeviceId();

	UE_LOG(LogBle, Verbose, TEXT("UBLEDevice::SetDeviceInternal() => Device: %s with Service: %s"), *DeviceId, *ServiceUUID);
	BLEDeviceHolder::GetInstance()->AddDevice(MakeTuple(DeviceId.ToLower(), ServiceUUID.ToLower()), this);
}


void UBLEDevice::SetManagerInternal(FGuid managerGuid)
{
	ManagerGuid = managerGuid;
}

FString UBLEDevice::GetDeviceId() 
{
	JNIEnv* Env = FAndroidApplication::GetJavaEnv();
	jstring Return = static_cast<jstring>(Env->CallObjectMethod(javaDevice, GetDeviceIDMethod));
	FString Result = BLEAndroidUtils::GetFString(Return);

	UE_LOG(LogBle, Verbose, TEXT("UBLEDevice::GetDeviceId() => %s"), *Result);
	return Result;
}

FString UBLEDevice::GetDeviceName()
{
	JNIEnv* Env = FAndroidApplication::GetJavaEnv();
	jstring Return = static_cast<jstring>(Env->CallObjectMethod(javaDevice, GetDeviceNameMethod));
	FString Result = BLEAndroidUtils::GetFString(Return);

	UE_LOG(LogBle, Verbose, TEXT("UBLEDevice::GetDeviceName() => %s"), *Result);
	return Result;
}

FString UBLEDevice::GetServiceUUID() const
{
	return ServiceUUID;
}

bool UBLEDevice::IsConnected()
{
	JNIEnv* Env = FAndroidApplication::GetJavaEnv();
	return Env->CallBooleanMethod(javaDevice, IsDeviceConnected);
}

void UBLEDevice::ReadCharacteristic(const FOnReadSuccessDelegate& onReadSuccessCallback,
	const FOnReadErrorDelegate& onReadErrorCallback, FString characteristicUUID)
{
	UE_LOG(LogBle, Verbose, TEXT("UBLEDevice::ReadCharacteristic() => %s"), *characteristicUUID);

	OnReadSuccessDelegates.Emplace(characteristicUUID.ToLower(), onReadSuccessCallback);
	OnReadErrorDelegates.Emplace(characteristicUUID.ToLower(), onReadErrorCallback);

	JNIEnv* Env = FAndroidApplication::GetJavaEnv();
	Env->CallVoidMethod(javaDevice, ReadCharacteristicMethod, BLEAndroidUtils::GetJavaString(characteristicUUID));
}

void UBLEDevice::WriteCharacteristic(const FOnWriteSuccessDelegate& onWriteSuccessCallback,
	const FOnWriteErrorDelegate& onWriteErrorCallback, FString characteristicUUID, TArray<uint8> data)
{
	UE_LOG(LogBle, Verbose, TEXT("UBLEDevice::WriteCharacteristic() => %s"), *characteristicUUID);

	OnWriteSuccessDelegates.Emplace(characteristicUUID.ToLower(), onWriteSuccessCallback);
	OnWriteErrorDelegates.Emplace(characteristicUUID.ToLower(), onWriteErrorCallback);

	JNIEnv* Env = FAndroidApplication::GetJavaEnv();
	Env->CallVoidMethod(javaDevice, WriteCharacteristicMethod, 
		BLEAndroidUtils::GetJavaString(characteristicUUID),
		BLEAndroidUtils::GetJavaArray(data));
}

void UBLEDevice::SubscribeForNotifications(const FOnNotificationReceivedDelegate& onNotificationReceivedCallback,
	const FOnNotificationErrorDelegate& onNotificationErrorCallback, FString characteristicUUID, bool isIndicationCharacteristic)
{
	UE_LOG(LogBle, Verbose, TEXT("UBLEDevice::SubscribeForNotifications() => %s"), *characteristicUUID);

	OnNotificationReceivedDelegates.Emplace(characteristicUUID.ToLower(), onNotificationReceivedCallback);
	OnNotificationErrorDelegates.Emplace(characteristicUUID.ToLower(), onNotificationErrorCallback);

	JNIEnv* Env = FAndroidApplication::GetJavaEnv();
	Env->CallVoidMethod(javaDevice, SubscribeForNotificationsMethod, BLEAndroidUtils::GetJavaString(characteristicUUID), isIndicationCharacteristic);
}

void UBLEDevice::StopNotifications(const FOnNotificationsStoppedDelegate& onNotificationsStoppedCallback,
	const FOnNotificationsStoppedErrorDelegate& onNotificationErrorCallback, FString characteristicUUID)
{
	UE_LOG(LogBle, Verbose, TEXT("UBLEDevice::StopNotifications() => %s"), *characteristicUUID);

	OnNotificationsStoppedDelegates.Emplace(characteristicUUID.ToLower(), onNotificationsStoppedCallback);
	OnNotificationsStoppedErrorDelegates.Emplace(characteristicUUID.ToLower(), onNotificationErrorCallback);

	JNIEnv* Env = FAndroidApplication::GetJavaEnv();
	Env->CallVoidMethod(javaDevice, StopNotificationsMethod, BLEAndroidUtils::GetJavaString(characteristicUUID));
}

void UBLEDevice::OnReadSuccess(FString uuid, TArray<uint8> bytes)
{
	if (OnReadSuccessDelegates.Contains(uuid))
	{
		OnReadSuccessDelegates[uuid].ExecuteIfBound(bytes);
	}
}

void UBLEDevice::OnReadError(FString uuid, FString error)
{
	if (OnReadErrorDelegates.Contains(uuid))
	{
		OnReadErrorDelegates[uuid].ExecuteIfBound(error);
	}
}

void UBLEDevice::OnWriteSuccess(FString uuid)
{
	if (OnWriteSuccessDelegates.Contains(uuid))
	{
		OnWriteSuccessDelegates[uuid].ExecuteIfBound();
	}
}

void UBLEDevice::OnWriteError(FString uuid, FString error)
{
	if (OnWriteErrorDelegates.Contains(uuid))
	{
		OnWriteErrorDelegates[uuid].ExecuteIfBound(error);
	}
}

void UBLEDevice::OnNotificationReceived(FString uuid, TArray<uint8> bytes)
{
	if (OnNotificationReceivedDelegates.Contains(uuid))
	{
		OnNotificationReceivedDelegates[uuid].ExecuteIfBound(bytes);
	}
}

void UBLEDevice::OnNotificationError(FString uuid, FString error)
{
	if (OnNotificationErrorDelegates.Contains(uuid))
	{
		OnNotificationErrorDelegates[uuid].ExecuteIfBound(error);
	}
}

void UBLEDevice::OnStopNotifications(FString uuid)
{
	if (OnNotificationsStoppedDelegates.Contains(uuid))
	{
		OnNotificationsStoppedDelegates[uuid].ExecuteIfBound();
	}
}

void UBLEDevice::OnNotificationStopError(FString uuid, FString error)
{
	if (OnNotificationsStoppedErrorDelegates.Contains(uuid))
	{
		OnNotificationsStoppedErrorDelegates[uuid].ExecuteIfBound(error);
	}
}
