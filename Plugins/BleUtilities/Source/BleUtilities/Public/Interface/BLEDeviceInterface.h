// Copyright (c) 2019 Nineva Studios

#pragma once

#include "UObject/Interface.h"
#include "BLEDeviceInterface.generated.h"

DECLARE_DYNAMIC_DELEGATE_OneParam(FOnNotificationReceivedDelegate, const TArray<uint8>&, data);
DECLARE_DYNAMIC_DELEGATE(FOnNotificationsStoppedDelegate);
DECLARE_DYNAMIC_DELEGATE_OneParam(FOnNotificationErrorDelegate, FString, error);

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
    * Check BLE connection status
    */
    UFUNCTION(BlueprintCallable, Category = "BleUtilities")
    virtual bool IsConnected() = 0;
    
    /**
    * Subscribe for BLE device Cycling Power Measurement notifications
    */
    UFUNCTION(BlueprintCallable, Category = "BleUtilities")
    virtual void SubscribeForNotifications(const FOnNotificationReceivedDelegate& onNotificationReceivedCallback,
        const FOnNotificationErrorDelegate& onNotificationErrorCallback) = 0;
    
    /**
    * Stop receiving BLE device Cycling Power Measurement notifications
    */
    UFUNCTION(BlueprintCallable, Category = "BleUtilities")
    virtual void StopNotifications(const FOnNotificationsStoppedDelegate& onNotificationsStoppedCallback,
        const FOnNotificationErrorDelegate& onNotificationErrorCallback) = 0;   
};
