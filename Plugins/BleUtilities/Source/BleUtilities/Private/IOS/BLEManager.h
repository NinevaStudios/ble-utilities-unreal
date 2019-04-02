// Copyright (c) 2019 Nineva Studios

#pragma once

#include "Interface/BLEManagerInterface.h"

#if PLATFORM_IOS
#import <CoreBluetooth/CoreBluetooth.h>
#endif

#include "BLEManager.generated.h"

UCLASS()
class UBLEManager : public UObject, public IBLEManagerInterface
{
	GENERATED_UCLASS_BODY()

public:
    
    virtual ~UBLEManager();
    
    virtual void PostInitProperties() override;
    virtual void BeginDestroy() override;
    
    UFUNCTION(BlueprintCallable, Category = "BleUtilities")
    virtual bool CheckBleSupport() override;
    
    UFUNCTION(BlueprintCallable, Category = "BleUtilities")
    virtual bool IsBluetoothEnabled() override;

	UFUNCTION(BlueprintCallable, Category = "BleUtilities")
	virtual void SetBluetoothState(bool enable) override;
    
    UFUNCTION(BlueprintCallable, Category = "BleUtilities")
    virtual void ScanForDevices(const FOnDeviceFoundDelegate& onDeviceFoundCallback) override;
    
    UFUNCTION(BlueprintCallable, Category = "BleUtilities")
    virtual void StopScan() override;
    
    UFUNCTION(BlueprintCallable, Category = "BleUtilities")
    virtual void ConnectToDevice(TScriptInterface<IBLEDeviceInterface> device,
        const FOnDeviceConnectedDelegate& onDeviceConnectedCallback, const FOnConnectionErrorDelegate& onFailToConnectDeviceCallback) override;
    
    UFUNCTION(BlueprintCallable, Category = "BleUtilities")
    virtual void DisconnectDevice(TScriptInterface<IBLEDeviceInterface> device,
        const FOnDeviceDisconnectedDelegate& onDeviceDisconnectedCallback, const FOnDisconnectionErrorDelegate& onFailToDosconnectDeviceCallback) override;
    
private:

	CBCentralManager* CentralManager;
    
    UPROPERTY()
    FOnDeviceFoundDelegate OnDeviceFoundDelegate;
    UPROPERTY()
    FOnDeviceConnectedDelegate OnDeviceConnectedDelegate;
    UPROPERTY()
    FOnConnectionErrorDelegate OnConnectionErrorDelegate;
    UPROPERTY()
    FOnDeviceDisconnectedDelegate OnDeviceDisconnectedDelegate;
    UPROPERTY()
    FOnDisconnectionErrorDelegate OnDisconnectionErrorDelegate;
};
