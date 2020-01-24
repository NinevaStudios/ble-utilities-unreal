// Copyright (c) 2019 Nineva Studios

#pragma once

#include "UObject/Interface.h"
#include "BLEDeviceInterface.generated.h"

DECLARE_DYNAMIC_DELEGATE_OneParam(FOnNotificationReceivedDelegate, const TArray<uint8>&, data);
DECLARE_DYNAMIC_DELEGATE(FOnNotificationsStoppedDelegate);
DECLARE_DYNAMIC_DELEGATE_OneParam(FOnNotificationErrorDelegate, FString, error);
DECLARE_DYNAMIC_DELEGATE_OneParam(FOnNotificationsStoppedErrorDelegate, FString, error);
DECLARE_DYNAMIC_DELEGATE_OneParam(FOnReadSuccessDelegate, const TArray<uint8>&, data);
DECLARE_DYNAMIC_DELEGATE_OneParam(FOnReadErrorDelegate, FString, error);
DECLARE_DYNAMIC_DELEGATE(FOnWriteSuccessDelegate);
DECLARE_DYNAMIC_DELEGATE_OneParam(FOnWriteErrorDelegate, FString, error);

UINTERFACE(BlueprintType, meta = (CannotImplementInterfaceInBlueprint))
class BLEUTILITIES_API UBLEDeviceInterface : public UInterface
{
	GENERATED_BODY()
};

class BLEUTILITIES_API IBLEDeviceInterface
{
	GENERATED_BODY()

public:

	/**
	* Get BLE device name
	*/
	UFUNCTION(BlueprintCallable, Category = "BleUtilities")
	virtual FString GetDeviceName() = 0;

	/**
	* Get BLE device Id
	*/
	UFUNCTION(BlueprintCallable, Category = "BleUtilities")
	virtual FString GetDeviceId() = 0;

	/**
	* Get BLE device service UUID
	*/
	UFUNCTION(BlueprintCallable, Category = "BleUtilities")
	virtual FString GetServiceUUID() const = 0;

	/**
	* Check BLE connection status
	*/
	UFUNCTION(BlueprintCallable, Category = "BleUtilities")
	virtual bool IsConnected() = 0;

	/**
	* Read service characteristic value
	*/
	UFUNCTION(BlueprintCallable, Category = "BleUtilities")
	virtual void ReadCharacteristic(const FOnReadSuccessDelegate& onReadSuccessCallback,
		const FOnReadErrorDelegate& onReadErrorCallback, FString characteristicUUID) = 0;

	/**
	* Write service characteristic value
	*/
	UFUNCTION(BlueprintCallable, Category = "BleUtilities")
	virtual void WriteCharacteristic(const FOnWriteSuccessDelegate& onWriteSuccessCallback,
		const FOnWriteErrorDelegate& onWriteErrorCallback, FString characteristicUUID, TArray<uint8> data) = 0;

	/**
	* Subscribe to a service characteristic to receive notifications
	*/
	UFUNCTION(BlueprintCallable, Category = "BleUtilities")
	virtual void SubscribeForNotifications(const FOnNotificationReceivedDelegate& onNotificationReceivedCallback,
		const FOnNotificationErrorDelegate& onNotificationErrorCallback, FString characteristicUUID, bool isIndicationCharacteristic) = 0;

	/**
	* Stop receiving service characteristic notifications
	*/
	UFUNCTION(BlueprintCallable, Category = "BleUtilities")
	virtual void StopNotifications(const FOnNotificationsStoppedDelegate& onNotificationsStoppedCallback,
		const FOnNotificationsStoppedErrorDelegate& onNotificationErrorCallback, FString characteristicUUID) = 0;
};
