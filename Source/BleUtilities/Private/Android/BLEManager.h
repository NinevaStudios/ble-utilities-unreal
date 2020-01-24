// Copyright (c) 2019 Nineva Studios

#pragma once

#include "Interface/BLEManagerInterface.h"

#if PLATFORM_ANDROID
#include "Android/AndroidJNI.h"
#include "Android/AndroidApplication.h"
#include "Android/AndroidJava.h"
#endif

#include "BLEManager.generated.h"

class UBLEDevice;

UCLASS()
class UBLEManager : public UObject, public IBLEManagerInterface
{
	GENERATED_UCLASS_BODY()

public:

	virtual ~UBLEManager() = default;

	virtual void SetServiceUUIDsToScanFor(TArray<FString> serviceUUIDs) override;

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
		const FOnDeviceDisconnectedDelegate& onDeviceDisconnectedCallback, const FOnDisconnectionErrorDelegate& onFailToDisconnectDeviceCallback) override;

	// Callbacks interface methods to be called from Android
	void OnDeviceFound(UBLEDevice* device);
	void OnDeviceConnected();
	void OnFailedToConnectDevice(FString error);
	void OnDeviceDisconnected();
	void OnFailedToDisconnectDevice(FString error);
private:

	// Unique BLE manager identifier
	FGuid Guid;

	TMap<FString, TSharedPtr<FJavaClassObject>> BLEManagerJavaObjectMap;

	// Java methods
	FJavaClassMethod StartScanMethod;
	FJavaClassMethod StopScanMethod;
	FJavaClassMethod IsBluetoothEnabledMethod;
	FJavaClassMethod IsBleSupportedMethod;
	FJavaClassMethod SetBluetoothEnabled;
	FJavaClassMethod ConnectToDeviceMethod;
	FJavaClassMethod DisconnectDeviceMethod;	

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
