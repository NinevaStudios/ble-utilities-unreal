// Copyright (c) 2019 Nineva Studios

#pragma once

#include "UObject/Interface.h"
#include "BLEManagerInterface.generated.h"

DECLARE_DYNAMIC_DELEGATE_OneParam(FOnDeviceFoundDelegate, TScriptInterface<IBLEDeviceInterface>, device);
DECLARE_DYNAMIC_DELEGATE(FOnDeviceConnectedDelegate);
DECLARE_DYNAMIC_DELEGATE(FOnDeviceDisconnectedDelegate);
DECLARE_DYNAMIC_DELEGATE_OneParam(FOnConnectionErrorDelegate, FString, error);
DECLARE_DYNAMIC_DELEGATE_OneParam(FOnDisconnectionErrorDelegate, FString, error);

UINTERFACE(BlueprintType, meta = (CannotImplementInterfaceInBlueprint))
class BLEUTILITIES_API UBLEManagerInterface : public UInterface
{
	GENERATED_BODY()
};

class BLEUTILITIES_API IBLEManagerInterface
{
	GENERATED_BODY()

public:
	
	virtual void SetServiceUUIDsToScanFor(TArray<FString> serviceUUIDs) = 0;

	/**
	* Check if BLE supported
	*/
	UFUNCTION(BlueprintCallable, Category = "BleUtilities")
	virtual bool CheckBleSupport() = 0;
	
	/**
	* Check if bluetooth is turned on
	*/
	UFUNCTION(BlueprintCallable, Category = "BleUtilities")
	virtual bool IsBluetoothEnabled() = 0;

	/**
	* Turn bluetooth on/off
	* @note Works ONLY for Android
	*/
	UFUNCTION(BlueprintCallable, Category = "BleUtilities")
	virtual void SetBluetoothState(bool enable) = 0;
	
	/**
	* Find nearby BLE devices with specific services
	*/
	UFUNCTION(BlueprintCallable, Category = "BleUtilities")
	virtual void ScanForDevices(const FOnDeviceFoundDelegate& onDeviceFoundCallback) = 0;

	/**
	* Stop searching for nearby BLE devices
	*/
	UFUNCTION(BlueprintCallable, Category = "BleUtilities")
	virtual void StopScan() = 0;
	
	/**
	* Connect to specified BLE device
	*/
	UFUNCTION(BlueprintCallable, Category = "BleUtilities")
	virtual void ConnectToDevice(TScriptInterface<IBLEDeviceInterface> device,
		const FOnDeviceConnectedDelegate& onDeviceConnectedCallback, const FOnConnectionErrorDelegate& onFailToConnectDeviceCallback) = 0;

	/**
	* Disconnect specified BLE device
	*/
	UFUNCTION(BlueprintCallable, Category = "BleUtilities")
	virtual void DisconnectDevice(TScriptInterface<IBLEDeviceInterface> device,
		const FOnDeviceDisconnectedDelegate& onDeviceDisconnectedCallback, const FOnDisconnectionErrorDelegate& onFailToDisconnectDeviceCallback) = 0;
};
