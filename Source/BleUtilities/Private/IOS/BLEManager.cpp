// Copyright (c) 2019 Nineva Studios

#include "BLEManager.h"

#include "BLEDevice.h"
#include "BleLog.h"

#import "BLEManagerDelegate.h"

UBLEManager::UBLEManager(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	UE_LOG(LogBle, Verbose, TEXT("UBLEManager::UBLEManager()"));

	if (UBLEManager::StaticClass()->GetDefaultObject() != this)
	{
		ManagerDelegate = [[BLEManagerDelegate alloc]init];

		ManagerDelegate.onDeviceFound = ^ (CBPeripheral * device, CBCentralManager * manager) {
			UBLEDevice* BLEDevice = NewObject<UBLEDevice>();

			CustomCBCentralManager* customManager = (CustomCBCentralManager*)manager;

			BLEDevice->SetDeviceInternal(device, customManager.serviceUUID);

			TScriptInterface<IBLEDeviceInterface> BLEDeviceInterface;
			BLEDeviceInterface.SetObject(BLEDevice);
			BLEDeviceInterface.SetInterface(Cast<IBLEDeviceInterface>(BLEDevice));

			OnDeviceFoundDelegate.ExecuteIfBound(BLEDevice);
		};
		ManagerDelegate.onDeviceConnected = ^ (CBPeripheral* device, CBCentralManager* manager) {
			CustomCBCentralManager* customManager = (CustomCBCentralManager*)manager;
			CBUUID* uuid = [CBUUID UUIDWithString: customManager.serviceUUID.GetNSString()];
			[device discoverServices:@[uuid]];
			OnDeviceConnectedDelegate.ExecuteIfBound();
		};
		ManagerDelegate.onFailToConnect = ^ (FString error) {
			OnConnectionErrorDelegate.ExecuteIfBound(error);
		};
		ManagerDelegate.onDeviceDisconnected = ^ () {
			OnDeviceDisconnectedDelegate.ExecuteIfBound();
		};
		ManagerDelegate.onFailToDisconnect = ^ (FString error) {
			OnDisconnectionErrorDelegate.ExecuteIfBound(error);
		};
	}
}

void UBLEManager::SetServiceUUIDsToScanFor(TArray<FString> serviceUUIDs)
{
	for (const FString& serviceUUID : serviceUUIDs)
	{
		CustomCBCentralManager* CentralManager = [[CustomCBCentralManager alloc]initWithDelegate:ManagerDelegate queue : nil];
		CentralManager.serviceUUID = serviceUUID;

		CentralManagerMap.Emplace(serviceUUID, CentralManager);
	}
}

void UBLEManager::PostInitProperties()
{
	Super::PostInitProperties();

	if (UBLEManager::StaticClass()->GetDefaultObject() != this)
	{
		// add BLE manager to root to avoid GC issues
		AddToRoot();
	}
}

void UBLEManager::BeginDestroy()
{
	UE_LOG(LogBle, Verbose, TEXT("UBLEManager::BeginDestroy()"));

	if (UBLEManager::StaticClass()->GetDefaultObject() != this)
	{
		// remove BLE manager from root so now it can be safely destroyed by GC
		RemoveFromRoot();
	}

	Super::BeginDestroy();
}

bool UBLEManager::CheckBleSupport()
{
	CustomCBCentralManager* CentralManager = CentralManagerMap.begin()->Value;
	return CentralManager.state != CBManagerStateUnsupported;
}

bool UBLEManager::IsBluetoothEnabled()
{
	CustomCBCentralManager* CentralManager = CentralManagerMap.begin()->Value;
	return CentralManager.state == CBManagerStatePoweredOn;
}

void UBLEManager::SetBluetoothState(bool enable)
{
	UE_LOG(LogBle, Warning, TEXT("This functionality is not supported for iOS platform"));
}

void UBLEManager::ScanForDevices(const FOnDeviceFoundDelegate& onDeviceFoundCallback)
{
	UE_LOG(LogBle, Verbose, TEXT("UBLEManager::ScanForDevices()"));

	OnDeviceFoundDelegate = onDeviceFoundCallback;
	for (const auto& CentralManagerPair : CentralManagerMap)
	{
		CustomCBCentralManager* CentralManager = CentralManagerPair.Value;
		if (CentralManager.state == CBManagerStatePoweredOn)
		{
			CBUUID* uuid = [CBUUID UUIDWithString: CentralManagerPair.Key.GetNSString()];
			[CentralManager scanForPeripheralsWithServices:@[uuid]
							options:@{CBCentralManagerScanOptionAllowDuplicatesKey : @NO}];
		}
	}
}

void UBLEManager::StopScan()
{
	UE_LOG(LogBle, Verbose, TEXT("UBLEManager::StopScan()"));
	for (const auto& CentralManagerPair : CentralManagerMap)
	{
		CustomCBCentralManager* CentralManager = CentralManagerPair.Value;
		[CentralManager stopScan];
	}
}

void UBLEManager::ConnectToDevice(TScriptInterface<IBLEDeviceInterface> device,
    const FOnDeviceConnectedDelegate& onDeviceConnectedCallback, const FOnConnectionErrorDelegate& onFailToConnectDeviceCallback)
{
    UE_LOG(LogBle, Verbose, TEXT("UBLEManager::ConnectToDevice()"));

    OnDeviceConnectedDelegate = onDeviceConnectedCallback;
    OnConnectionErrorDelegate = onFailToConnectDeviceCallback;

    UBLEDevice* BLEDevice = dynamic_cast<UBLEDevice*>(device.GetObject());

    if(!BLEDevice)
    {
        UE_LOG(LogBle, Error, TEXT("Failed to cast BLE device pointer"));
        return;
    }

	CustomCBCentralManager* CentralManager = CentralManagerMap[BLEDevice->GetServiceUUID()];
	[CentralManager connectPeripheral : BLEDevice->GetDeviceInternal() options : nil];
}

void UBLEManager::DisconnectDevice(TScriptInterface<IBLEDeviceInterface> device,
    const FOnDeviceDisconnectedDelegate& onDeviceDisconnectedCallback, const FOnDisconnectionErrorDelegate& onFailToDisconnectDeviceCallback)
{
    UE_LOG(LogBle, Verbose, TEXT("UBLEManager::DisconnectDevice()"));

    OnDeviceDisconnectedDelegate = onDeviceDisconnectedCallback;
    OnDisconnectionErrorDelegate = onFailToDisconnectDeviceCallback;

    UBLEDevice* BLEDevice = dynamic_cast<UBLEDevice*>(device.GetObject());

    if(!BLEDevice)
    {
        UE_LOG(LogBle, Error, TEXT("Failed to cast BLE device pointer"));
        return;
    }

	CustomCBCentralManager* CentralManager = CentralManagerMap[BLEDevice->GetServiceUUID()];
	[CentralManager cancelPeripheralConnection : BLEDevice->GetDeviceInternal()];
}
