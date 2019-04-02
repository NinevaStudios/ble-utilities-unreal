// Copyright (c) 2019 Nineva Studios

#pragma once

#include "Interface/BLEDeviceInterface.h"

#if PLATFORM_IOS
#import <CoreBluetooth/CoreBluetooth.h>
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
    virtual bool IsConnected() override;
    
    UFUNCTION(BlueprintCallable, Category = "BleUtilities")
    virtual void SubscribeForNotifications(const FOnNotificationReceivedDelegate& onNotificationReceivedCallback,
        const FOnNotificationErrorDelegate& onNotificationErrorCallback) override;
    
    UFUNCTION(BlueprintCallable, Category = "BleUtilities")
    virtual void StopNotifications(const FOnNotificationsStoppedDelegate& onNotificationsStoppedCallback,
        const FOnNotificationErrorDelegate& onNotificationErrorCallback) override;
    
public:
    
    void SetDeviceInternal(CBPeripheral* device);
    CBPeripheral* GetDeviceInternal();
    
private:
    
    CBPeripheral* Device;
    
    UPROPERTY()
    FOnNotificationReceivedDelegate OnNotificationReceivedDelegate;
    UPROPERTY()
    FOnNotificationsStoppedDelegate OnNotificationsStoppedDelegate;
    UPROPERTY()
    FOnNotificationErrorDelegate OnNotificationErrorDelegate;
};
