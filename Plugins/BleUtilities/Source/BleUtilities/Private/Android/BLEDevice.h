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

	virtual ~UBLEDevice();
	
	virtual void PostInitProperties() override;
	virtual void BeginDestroy() override;

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

	// Callbacks interface methods to be called from Android
	void OnNotificationReceived(TArray<uint8> bytes);
	void OnStopNotifications();
	void OnNotificationError(FString error);

	void SetDeviceInternal(FString deviceId);
	FString GetDeviceInternal();

	void SetManagerInternal(FGuid managerGuid);

	FGuid GetDeviceGuid();

private:

	// Unique device identifier
	FGuid DeviceGuid;
	// Unique manager identifier that holds current device
	FGuid ManagerGuid;

	FString DeviceId;

	UPROPERTY()
	FOnNotificationReceivedDelegate OnNotificationReceivedDelegate;
	UPROPERTY()
	FOnNotificationsStoppedDelegate OnNotificationsStoppedDelegate;
	UPROPERTY()
	FOnNotificationErrorDelegate OnNotificationErrorDelegate;
};
