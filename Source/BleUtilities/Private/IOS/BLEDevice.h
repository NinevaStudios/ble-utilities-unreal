// Copyright (c) 2019 Nineva Studios

#pragma once

#include "Interface/BLEDeviceInterface.h"

#if PLATFORM_IOS
#import <CoreBluetooth/CoreBluetooth.h>
@class BLEDeviceDelegate;
#endif

#include "BLEDevice.generated.h"

UCLASS()
class UBLEDevice : public UObject, public IBLEDeviceInterface
{
    GENERATED_UCLASS_BODY()

public:

    virtual ~UBLEDevice();

    UFUNCTION(BlueprintCallable, Category = "BleUtilities")
    virtual FString GetDeviceName() override;

    UFUNCTION(BlueprintCallable, Category = "BleUtilities")
    virtual FString GetDeviceId() override;

	UFUNCTION(BlueprintCallable, Category = "BleUtilities")
	virtual FString GetServiceUUID() const override;

    UFUNCTION(BlueprintCallable, Category = "BleUtilities")
    virtual bool IsConnected() override;

	UFUNCTION(BlueprintCallable, Category = "BleUtilities")
	virtual void ReadCharacteristic(const FOnReadSuccessDelegate& onReadSuccessCallback,
		const FOnReadErrorDelegate& onReadErrorCallback, FString characteristicUUID) override;

	UFUNCTION(BlueprintCallable, Category = "BleUtilities")
	virtual void WriteCharacteristic(const FOnWriteSuccessDelegate& onWriteSuccessCallback,
		const FOnWriteErrorDelegate& onWriteErrorCallback, FString characteristicUUID, TArray<uint8> data) override;

    UFUNCTION(BlueprintCallable, Category = "BleUtilities")
    virtual void SubscribeForNotifications(const FOnNotificationReceivedDelegate& onNotificationReceivedCallback,
        const FOnNotificationErrorDelegate& onNotificationErrorCallback, FString characteristicUUID, bool isIndicationCharacteristic) override;

    UFUNCTION(BlueprintCallable, Category = "BleUtilities")
    virtual void StopNotifications(const FOnNotificationsStoppedDelegate& onNotificationsStoppedCallback,
        const FOnNotificationsStoppedErrorDelegate& onNotificationErrorCallback, FString characteristicUUID) override;

public:
	void SetDeviceInternal(CBPeripheral* device, FString serviceUUID);
	CBPeripheral* GetDeviceInternal();

private:
	CBCharacteristic* GetServiceCharacteristic(FString characteristicUUID) const;

	FString ServiceUUID;
	BLEDeviceDelegate* DeviceDelegate;
	CBPeripheral* Device;

	TMap<FString, FOnReadSuccessDelegate> OnReadSuccessDelegates;
	TMap<FString, FOnReadErrorDelegate> OnReadErrorDelegates;
	TMap<FString, FOnWriteSuccessDelegate> OnWriteSuccessDelegates;
	TMap<FString, FOnWriteErrorDelegate> OnWriteErrorDelegates;
	TMap<FString, FOnNotificationReceivedDelegate> OnNotificationReceivedDelegates;
	TMap<FString, FOnNotificationErrorDelegate> OnNotificationErrorDelegates;
	TMap<FString, FOnNotificationsStoppedDelegate> OnNotificationsStoppedDelegates;
	TMap<FString, FOnNotificationsStoppedErrorDelegate> OnNotificationsStoppedErrorDelegates;
};
