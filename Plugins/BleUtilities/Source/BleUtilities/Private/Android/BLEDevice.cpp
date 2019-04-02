// Copyright (c) 2019 Nineva Studios

#include "BLEDevice.h"
#include "BLEManager.h"
#include "BLEDeviceHolder.h"
#include "BLEManagerHolder.h"

UBLEDevice::UBLEDevice(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
	, DeviceGuid(FGuid::NewGuid())
{
	UE_LOG(LogTemp, Warning, TEXT("BLEUTILS => UBLEDevice::UBLEManager()"));
}

UBLEDevice::~UBLEDevice()
{
	UE_LOG(LogTemp, Warning, TEXT("BLEUTILS => UBLEDevice::~UBLEDevice()"));
}

void UBLEDevice::PostInitProperties()
{
	UE_LOG(LogTemp, Warning, TEXT("BLEUTILS => UBLEManager::PostInitProperties()"));

	Super::PostInitProperties();

	if (UBLEDevice::StaticClass()->GetDefaultObject() != this)
	{
		// add BLE manager to root to avoid GC issues
		AddToRoot();

		BLEDeviceHolder::GetInstance()->AddDevice(DeviceGuid, this);
	}
}

void UBLEDevice::BeginDestroy()
{
	UE_LOG(LogTemp, Warning, TEXT("BLEUTILS => UBLEManager::BeginDestroy()"));

	if (UBLEDevice::StaticClass()->GetDefaultObject() != this)
	{
		BLEDeviceHolder::GetInstance()->RemoveDevice(DeviceGuid);

		// remove BLE manager from root so now it can be safely destroyed by GC
		RemoveFromRoot();
	}

	Super::BeginDestroy();
}

void UBLEDevice::SetDeviceInternal(FString deviceId)
{
	UE_LOG(LogTemp, Warning, TEXT("BLEUTILS => UBLEDevice::SetDeviceInternal()"));

	DeviceId = deviceId;
}

FString UBLEDevice::GetDeviceInternal()
{
	UE_LOG(LogTemp, Warning, TEXT("BLEUTILS => UBLEDevice::GetDeviceInternal()"));

	return DeviceId;
}


void UBLEDevice::SetManagerInternal(FGuid managerGuid)
{
	ManagerGuid = managerGuid;
}


FGuid UBLEDevice::GetDeviceGuid()
{
	return DeviceGuid;
}

FString UBLEDevice::GetDeviceName()
{
	UE_LOG(LogTemp, Warning, TEXT("BLEUTILS => UBLEDevice::GetDeviceName()"));

	FString Result;

	auto bleManagerHolder = BLEManagerHolder::GetInstance();
	auto manager = dynamic_cast<UBLEManager*>(bleManagerHolder->GetManager(ManagerGuid));
	if (manager)
	{
		Result = manager->GetDeviceName(DeviceId);
	}

	return Result;
}

FString UBLEDevice::GetDeviceId()
{
	UE_LOG(LogTemp, Warning, TEXT("BLEUTILS => UBLEDevice::GetDeviceId()"));

	return DeviceId;
}

bool UBLEDevice::IsConnected()
{
	UE_LOG(LogTemp, Warning, TEXT("BLEUTILS => UBLEDevice::IsConnected()"));

	bool Result = false;

	auto bleManagerHolder = BLEManagerHolder::GetInstance();
	auto manager = dynamic_cast<UBLEManager*>(bleManagerHolder->GetManager(ManagerGuid));
	if (manager)
	{
		Result = manager->CheckIsDeviceConnected(DeviceId);
	}

	return Result;
}

void UBLEDevice::SubscribeForNotifications(const FOnNotificationReceivedDelegate& onNotificationReceivedCallback,
	const FOnNotificationErrorDelegate& onNotificationErrorCallback)
{
	UE_LOG(LogTemp, Warning, TEXT("BLEUTILS => UBLEDevice::SubscribeForNotifications()"));

	OnNotificationReceivedDelegate = onNotificationReceivedCallback;
	OnNotificationErrorDelegate = onNotificationErrorCallback;

	auto bleManagerHolder = BLEManagerHolder::GetInstance();
	auto manager = dynamic_cast<UBLEManager*>(bleManagerHolder->GetManager(ManagerGuid));
	if (manager)
	{
		manager->SubscribeForNotifications(DeviceId);
	}
}

void UBLEDevice::StopNotifications(const FOnNotificationsStoppedDelegate& onNotificationsStoppedCallback,
	const FOnNotificationErrorDelegate& onNotificationErrorCallback)
{
	UE_LOG(LogTemp, Warning, TEXT("BLEUTILS => UBLEDevice::StopNotifications()"));

	OnNotificationsStoppedDelegate = onNotificationsStoppedCallback;
	OnNotificationErrorDelegate = onNotificationErrorCallback;

	auto bleManagerHolder = BLEManagerHolder::GetInstance();
	auto manager = dynamic_cast<UBLEManager*>(bleManagerHolder->GetManager(ManagerGuid));
	if (manager)
	{
		manager->StopNotifications(DeviceId);
	}
}

void UBLEDevice::OnNotificationReceived(TArray<uint8> bytes)
{
	OnNotificationReceivedDelegate.ExecuteIfBound(bytes);
}

void UBLEDevice::OnStopNotifications()
{
	OnNotificationsStoppedDelegate.ExecuteIfBound();
}

void UBLEDevice::OnNotificationError(FString error)
{
	OnNotificationErrorDelegate.ExecuteIfBound(error);
}
