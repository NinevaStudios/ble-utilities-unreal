// Copyright (c) 2019 Nineva Studios

#include "BLEDevice.h"
#include "BleLog.h"

#import "BLEDeviceDelegate.h"

FString ConvertToFullUUID(FString shortUUID)
{
	FString fullUuid = "";
	if (shortUUID.Len() == 4) // this is a 16bit representation
	{
		fullUuid += "0000";
		fullUuid += shortUUID;
		fullUuid += "-0000-1000-8000-00805F9B34FB";
	}
	else if (shortUUID.Len() == 8) // this is a 32bit representation
	{
		fullUuid += shortUUID;
		fullUuid += "-0000-1000-8000-00805F9B34FB";
	}

	return fullUuid.ToLower();
}

UBLEDevice::UBLEDevice(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	UE_LOG(LogBle, Verbose, TEXT("UBLEDevice::UBLEDevice()"));

	if (UBLEDevice::StaticClass()->GetDefaultObject() != this)
	{
		DeviceDelegate = [[BLEDeviceDelegate alloc]init];

		DeviceDelegate.onWriteSuccess = ^ (FString uuid) {
			FString fullUuid = ConvertToFullUUID(uuid);
			if (OnWriteSuccessDelegates.Contains(fullUuid))
			{
				OnWriteSuccessDelegates[fullUuid].ExecuteIfBound();
			}
		};
		DeviceDelegate.onValueUpdate = ^ (FString uuid, NSData * data) {
			TArray<uint8> byteData;
			uint8* byteArray = (uint8*)data.bytes;
			for (int i = 0; i < data.length; i++)
			{
				byteData.Add(byteArray[i]);
			}

			FString fullUuid = ConvertToFullUUID(uuid);
			if (OnReadSuccessDelegates.Contains(fullUuid))
			{
				OnReadSuccessDelegates[fullUuid].ExecuteIfBound(byteData);
			}

			if (OnNotificationReceivedDelegates.Contains(fullUuid))
			{
				OnNotificationReceivedDelegates[fullUuid].ExecuteIfBound(byteData);
			}
		};
		DeviceDelegate.onStopNotifications = ^ (FString uuid) {
			FString fullUuid = ConvertToFullUUID(uuid);
			if (OnNotificationsStoppedDelegates.Contains(fullUuid))
			{
				OnNotificationsStoppedDelegates[fullUuid].ExecuteIfBound();
			}
		};
		DeviceDelegate.onError = ^ (BLEDeviceError type, FString uuid, FString error) {
			FString fullUuid = ConvertToFullUUID(uuid);
			switch (type)
			{
			case BLEDeviceError::Read:
				if (OnReadErrorDelegates.Contains(fullUuid))
				{
					OnReadErrorDelegates[fullUuid].ExecuteIfBound(error);
				}
				if (OnNotificationErrorDelegates.Contains(fullUuid))
				{
					OnNotificationErrorDelegates[fullUuid].ExecuteIfBound(error);
				}
				break;
			case BLEDeviceError::Write:
				if (OnWriteErrorDelegates.Contains(fullUuid))
				{
					OnWriteErrorDelegates[fullUuid].ExecuteIfBound(error);
				}
				break;
			case BLEDeviceError::Notify:
				if (OnNotificationsStoppedErrorDelegates.Contains(fullUuid))
				{
					OnNotificationsStoppedErrorDelegates[fullUuid].ExecuteIfBound(error);
				}
				break;
			default:
				UE_LOG(LogBle, Error, TEXT("Device Error: %s"), *error);
			}
		};
	}
}

UBLEDevice::~UBLEDevice()
{
	UE_LOG(LogBle, Verbose, TEXT("UBLEDevice::~UBLEDevice()"));

	if(Device)
	{
		Device = (CBPeripheral*)CFBridgingRelease(Device);
	}
}

void UBLEDevice::SetDeviceInternal(CBPeripheral* device, FString serviceUUID)
{
	Device = device;
	Device.delegate = DeviceDelegate;
	ServiceUUID = serviceUUID;

	UE_LOG(LogBle, Verbose, TEXT("UBLEDevice::SetDeviceInternal() => Device with service: %s"), *ServiceUUID);
}

CBPeripheral* UBLEDevice::GetDeviceInternal()
{
	return Device;
}

FString UBLEDevice::GetDeviceName()
{
	return FString(Device.name);
}

FString UBLEDevice::GetDeviceId()
{
	return FString([Device.identifier UUIDString]);
}

FString UBLEDevice::GetServiceUUID() const
{
	return ServiceUUID;
}

bool UBLEDevice::IsConnected()
{
	return Device.state == CBPeripheralStateConnected;
}

CBCharacteristic* UBLEDevice::GetServiceCharacteristic(FString characteristicUUID) const
{
	CBUUID* uuid = [CBUUID UUIDWithString:characteristicUUID.GetNSString()];
	if (Device.services != nil) {
		for (CBService* service in Device.services) {
			if (service.characteristics != nil) {
				for (CBCharacteristic* characteristic in service.characteristics) {
					if ([characteristic.UUID isEqual:uuid]) {
						return characteristic;
					}
				}
			}
		}
	}
	return nullptr;
}

void UBLEDevice::ReadCharacteristic(const FOnReadSuccessDelegate& onReadSuccessCallback,
	const FOnReadErrorDelegate& onReadErrorCallback, FString characteristicUUID)
{
	OnReadSuccessDelegates.Emplace(characteristicUUID.ToLower(), onReadSuccessCallback);
	OnReadErrorDelegates.Emplace(characteristicUUID.ToLower(), onReadErrorCallback);
	
	CBCharacteristic* characteristic = GetServiceCharacteristic(characteristicUUID);
	if (characteristic)
	{
		[Device readValueForCharacteristic:characteristic] ;
		UE_LOG(LogBle, Verbose, TEXT("UBLEDevice::ReadCharacteristic() => %s"), *characteristicUUID);
	}
}

void UBLEDevice::WriteCharacteristic(const FOnWriteSuccessDelegate& onWriteSuccessCallback,
	const FOnWriteErrorDelegate& onWriteErrorCallback, FString characteristicUUID, TArray<uint8> data)
{
	OnWriteSuccessDelegates.Emplace(characteristicUUID.ToLower(), onWriteSuccessCallback);
	OnWriteErrorDelegates.Emplace(characteristicUUID.ToLower(), onWriteErrorCallback);

	CBCharacteristic* characteristic = GetServiceCharacteristic(characteristicUUID);
	if (characteristic)
	{
		[Device writeValue:[NSData dataWithBytes:(void*)data.GetData() length:data.Num()] 
			forCharacteristic:characteristic type:CBCharacteristicWriteWithResponse];
		UE_LOG(LogBle, Verbose, TEXT("UBLEDevice::WriteCharacteristic() => %s"), *characteristicUUID);
	}
}

void UBLEDevice::SubscribeForNotifications(const FOnNotificationReceivedDelegate& onNotificationReceivedCallback,
	const FOnNotificationErrorDelegate& onNotificationErrorCallback, FString characteristicUUID, bool isIndicationCharacteristic)
{
	OnNotificationReceivedDelegates.Emplace(characteristicUUID.ToLower(), onNotificationReceivedCallback);
	OnNotificationErrorDelegates.Emplace(characteristicUUID.ToLower(), onNotificationErrorCallback);

	CBCharacteristic* characteristic = GetServiceCharacteristic(characteristicUUID);
	if (characteristic)
	{
		[Device setNotifyValue:YES forCharacteristic:characteristic];
		UE_LOG(LogBle, Verbose, TEXT("UBLEDevice::SubscribeForNotifications() => %s"), *characteristicUUID);
	}
}

void UBLEDevice::StopNotifications(const FOnNotificationsStoppedDelegate& onNotificationsStoppedCallback,
	const FOnNotificationsStoppedErrorDelegate& onNotificationErrorCallback, FString characteristicUUID)
{
	OnNotificationsStoppedDelegates.Emplace(characteristicUUID.ToLower(), onNotificationsStoppedCallback);
	OnNotificationsStoppedErrorDelegates.Emplace(characteristicUUID.ToLower(), onNotificationErrorCallback);

	CBCharacteristic* characteristic = GetServiceCharacteristic(characteristicUUID);
	if (characteristic)
	{
		[Device setNotifyValue:NO forCharacteristic:characteristic];
		UE_LOG(LogBle, Verbose, TEXT("UBLEDevice::StopNotifications() => %s"), *characteristicUUID);
	}
}
