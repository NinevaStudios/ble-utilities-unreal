// Copyright (c) 2019 Nineva Studios

#pragma once

#include "Interface/BLEDeviceInterface.h"

#include "Android/AndroidJNI.h"
#include "Android/AndroidApplication.h"
#include "Android/AndroidJava.h"

#include "BLEDevice.generated.h"

UCLASS()
class UBLEDevice : public UObject, public IBLEDeviceInterface
{
	GENERATED_UCLASS_BODY()

public:

	virtual ~UBLEDevice() = default;
	
	virtual void PostInitProperties() override;
	virtual void BeginDestroy() override;

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

	// Callbacks interface methods to be called from Android
	void OnReadSuccess(FString uuid, TArray<uint8> bytes);
	void OnReadError(FString uuid, FString error);
	void OnWriteSuccess(FString uuid);
	void OnWriteError(FString uuid, FString error);
	void OnNotificationReceived(FString uuid, TArray<uint8> bytes);
	void OnNotificationError(FString uuid, FString error);
	void OnStopNotifications(FString uuid);
	void OnNotificationStopError(FString uuid, FString error);
	void SetDeviceInternal(jobject jDevice, FString serviceUUID);
	void SetManagerInternal(FGuid managerGuid);

private:
	jobject javaDevice;
	// Unique device identifier
	FString DeviceId;
	// Unique manager identifier that holds current device
	FGuid ManagerGuid;
	FString ServiceUUID;

	jmethodID IsDeviceConnected;
	jmethodID GetDeviceNameMethod;
	jmethodID GetDeviceIDMethod;
	jmethodID SetDeviceIDMethod;
	jmethodID ReadCharacteristicMethod;
	jmethodID WriteCharacteristicMethod;
	jmethodID SubscribeForNotificationsMethod;
	jmethodID StopNotificationsMethod;

	TMap<FString, FOnReadSuccessDelegate> OnReadSuccessDelegates;
	TMap<FString, FOnReadErrorDelegate> OnReadErrorDelegates;
	TMap<FString, FOnWriteSuccessDelegate> OnWriteSuccessDelegates;
	TMap<FString, FOnWriteErrorDelegate> OnWriteErrorDelegates;
	TMap<FString, FOnNotificationReceivedDelegate> OnNotificationReceivedDelegates;
	TMap<FString, FOnNotificationErrorDelegate> OnNotificationErrorDelegates;
	TMap<FString, FOnNotificationsStoppedDelegate> OnNotificationsStoppedDelegates;
	TMap<FString, FOnNotificationsStoppedErrorDelegate> OnNotificationsStoppedErrorDelegates;
};
