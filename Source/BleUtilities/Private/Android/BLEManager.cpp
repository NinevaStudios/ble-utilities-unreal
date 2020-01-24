// Copyright (c) 2018 Nineva Studios

#include "BLEManager.h"

#include "BLEDevice.h"
#include "BLEManagerHolder.h"
#include "BleLog.h"
#include "Utils/BLEAndroidUtils.h"

UBLEManager::UBLEManager(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
	, Guid(FGuid::NewGuid())
{
	UE_LOG(LogBle, Verbose, TEXT("UBLEManager::UBLEManager()"));
}

void UBLEManager::SetServiceUUIDsToScanFor(TArray<FString> serviceUUIDs)
{
	bool initJavaBindings = true;
	for (const FString& serviceUUID : serviceUUIDs)
	{
		// create manager
		BLEManagerJavaObjectMap.Emplace(serviceUUID, MakeShareable(
			new FJavaClassObject(
				"com/ninevastudios/androidbleunrealapi/BleUtilitiesManager",
				"(Landroid/app/Activity;Ljava/lang/String;Ljava/lang/String;)V",
				FJavaWrapper::GameActivityThis, BLEAndroidUtils::GetJavaString(Guid.ToString()), BLEAndroidUtils::GetJavaString(serviceUUID))));

		if (initJavaBindings)
		{
			// get references to Java methods
			StartScanMethod = BLEManagerJavaObjectMap[serviceUUID]->GetClassMethod("startScan", "()V");
			StopScanMethod = BLEManagerJavaObjectMap[serviceUUID]->GetClassMethod("stopScan", "()V");
			IsBluetoothEnabledMethod = BLEManagerJavaObjectMap[serviceUUID]->GetClassMethod("isBluetoothEnabled", "()Z");
			IsBleSupportedMethod = BLEManagerJavaObjectMap[serviceUUID]->GetClassMethod("supportsBle", "()Z");
			SetBluetoothEnabled = BLEManagerJavaObjectMap[serviceUUID]->GetClassMethod("setBluetoothEnabled", "(Z)V");
			ConnectToDeviceMethod = BLEManagerJavaObjectMap[serviceUUID]->GetClassMethod("connect", "(Ljava/lang/String;)V");
			DisconnectDeviceMethod = BLEManagerJavaObjectMap[serviceUUID]->GetClassMethod("disconnect", "(Ljava/lang/String;)V");
			initJavaBindings = false;
		}
	}	
}

void UBLEManager::PostInitProperties()
{
	Super::PostInitProperties();

	if (UBLEManager::StaticClass()->GetDefaultObject() != this)
	{
		// add BLE manager to root to avoid GC issues
		AddToRoot();
		
		BLEManagerHolder::GetInstance()->AddManager(Guid, this);
	}
}

void UBLEManager::BeginDestroy()
{
	UE_LOG(LogBle, Verbose, TEXT("UBLEManager::BeginDestroy()"));

	if (UBLEManager::StaticClass()->GetDefaultObject() != this)
	{
		BLEManagerHolder::GetInstance()->RemoveManager(Guid);

		// remove BLE manager from root so now it can be safely destroyed by GC
		RemoveFromRoot();
	}

	Super::BeginDestroy();
}

bool UBLEManager::CheckBleSupport()
{
	const auto BLEManagerJavaObject = BLEManagerJavaObjectMap.begin()->Value;
	return BLEManagerJavaObject->CallMethod<bool>(IsBleSupportedMethod);
}

bool UBLEManager::IsBluetoothEnabled()
{
	const auto BLEManagerJavaObject = BLEManagerJavaObjectMap.begin()->Value;
	return BLEManagerJavaObject->CallMethod<bool>(IsBluetoothEnabledMethod);
}

void UBLEManager::SetBluetoothState(bool enable)
{
	const auto BLEManagerJavaObject = BLEManagerJavaObjectMap.begin()->Value;
	BLEManagerJavaObject->CallMethod<void>(SetBluetoothEnabled, enable);
}

void UBLEManager::ScanForDevices(const FOnDeviceFoundDelegate& onDeviceFoundCallback)
{
	UE_LOG(LogBle, Verbose, TEXT("UBLEManager::ScanForDevices()"));

	OnDeviceFoundDelegate = onDeviceFoundCallback;
	for (const auto& BLEManagerJavaObjectPair : BLEManagerJavaObjectMap)
	{
		BLEManagerJavaObjectPair.Value->CallMethod<void>(StartScanMethod);
	}
}

void UBLEManager::StopScan()
{
	UE_LOG(LogBle, Verbose, TEXT("UBLEManager::StopScan()"));

	for (const auto& BLEManagerJavaObjectPair : BLEManagerJavaObjectMap)
	{
		BLEManagerJavaObjectPair.Value->CallMethod<void>(StopScanMethod);
	}
}

void UBLEManager::ConnectToDevice(TScriptInterface<IBLEDeviceInterface> device,
	const FOnDeviceConnectedDelegate& onDeviceConnectedCallback, const FOnConnectionErrorDelegate& onFailToConnectDeviceCallback)
{
	UE_LOG(LogBle, Verbose, TEXT("UBLEManager::ConnectToDevice()"));

	OnDeviceConnectedDelegate = onDeviceConnectedCallback;
	OnConnectionErrorDelegate = onFailToConnectDeviceCallback;

	UBLEDevice* BLEDevice = dynamic_cast<UBLEDevice*>(device.GetObject());
	if (BLEDevice && BLEManagerJavaObjectMap.Contains(BLEDevice->GetServiceUUID()))
	{
		BLEManagerJavaObjectMap[BLEDevice->GetServiceUUID()]->CallMethod<void>(ConnectToDeviceMethod, BLEAndroidUtils::GetJavaString(BLEDevice->GetDeviceId()));
	}
	else
	{
		UE_LOG(LogBle, Error, TEXT("Tried to connect to an invalid device"));
	}
}

void UBLEManager::DisconnectDevice(TScriptInterface<IBLEDeviceInterface> device,
	const FOnDeviceDisconnectedDelegate& onDeviceDisconnectedCallback, const FOnDisconnectionErrorDelegate& onFailToDisconnectDeviceCallback)
{
	UE_LOG(LogBle, Verbose, TEXT("BLEUTILS => UBLEManager::DisconnectDevice()"));

	OnDeviceDisconnectedDelegate = onDeviceDisconnectedCallback;
	OnDisconnectionErrorDelegate = onFailToDisconnectDeviceCallback;

	UBLEDevice* BLEDevice = dynamic_cast<UBLEDevice*>(device.GetObject());

	if (BLEDevice && BLEManagerJavaObjectMap.Contains(BLEDevice->GetServiceUUID()))
	{
		BLEManagerJavaObjectMap[BLEDevice->GetServiceUUID()]->CallMethod<void>(DisconnectDeviceMethod, BLEAndroidUtils::GetJavaString(BLEDevice->GetDeviceId()));
	}
	else
	{
		UE_LOG(LogBle, Error, TEXT("Tried to disconnect from an invalid device"));
	}
}

void UBLEManager::OnDeviceFound(UBLEDevice* device)
{
	device->SetManagerInternal(Guid);

	OnDeviceFoundDelegate.ExecuteIfBound(device);	
}

void UBLEManager::OnDeviceConnected()
{
	OnDeviceConnectedDelegate.ExecuteIfBound();
}

void UBLEManager::OnFailedToConnectDevice(FString error)
{
	OnConnectionErrorDelegate.ExecuteIfBound(error);
}

void UBLEManager::OnDeviceDisconnected()
{
	OnDeviceDisconnectedDelegate.ExecuteIfBound();
}

void UBLEManager::OnFailedToDisconnectDevice(FString error)
{
	OnDisconnectionErrorDelegate.ExecuteIfBound(error);
}

