// Copyright (c) 2019 Nineva Studios

#include "BLEManager.h"
#include "BLEDevice.h"

#import "BLEManagerDelegate.h"

UBLEManager::UBLEManager(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	UE_LOG(LogTemp, Warning, TEXT("BLEUTILS => UBLEManager::UBLEManager()"));

	if (UBLEManager::StaticClass()->GetDefaultObject() != this)
	{
        BLEManagerDelegate* bleManagerDelegate = [[BLEManagerDelegate alloc] init];
        
        bleManagerDelegate.onDeviceFound = ^(CBPeripheral* device) {
            UBLEDevice* BLEDevice = NewObject<UBLEDevice>();
            BLEDevice->SetDeviceInternal(device);
            
            TScriptInterface<IBLEDeviceInterface> BLEDeviceInterface;
            BLEDeviceInterface.SetObject(BLEDevice);
            BLEDeviceInterface.SetInterface(Cast<IBLEDeviceInterface>(BLEDevice));
            
            OnDeviceFoundDelegate.ExecuteIfBound(BLEDevice);
        };
        bleManagerDelegate.onDeviceConnected = ^() {
            OnDeviceConnectedDelegate.ExecuteIfBound();
        };
        bleManagerDelegate.onFailToConnect = ^(NSError* error) {
            OnConnectionErrorDelegate.ExecuteIfBound(FString(error.localizedDescription));
        };
        bleManagerDelegate.onDeviceDisconnected = ^() {
            OnDeviceDisconnectedDelegate.ExecuteIfBound();
        };
        bleManagerDelegate.onFailToDisconnect = ^(NSError* error) {
            OnDisconnectionErrorDelegate.ExecuteIfBound(FString(error.localizedDescription));
        };
        
        CentralManager = [[CBCentralManager alloc] initWithDelegate:bleManagerDelegate queue:nil];
	}
}

UBLEManager::~UBLEManager()
{
	UE_LOG(LogTemp, Warning, TEXT("BLEUTILS => UBLEManager::~UBLEManager()"));
}

void UBLEManager::PostInitProperties()
{
	UE_LOG(LogTemp, Warning, TEXT("BLEUTILS => UBLEManager::PostInitProperties()"));

	Super::PostInitProperties();

	if (UBLEManager::StaticClass()->GetDefaultObject() != this)
	{
		// add BLE manager to root to avoid GC issues
		AddToRoot();
	}
}

void UBLEManager::BeginDestroy()
{
	UE_LOG(LogTemp, Warning, TEXT("BLEUTILS => UBLEManager::BeginDestroy()"));

	if (UBLEManager::StaticClass()->GetDefaultObject() != this)
	{
		// remove BLE manager from root so now it can be safely destroyed by GC
		RemoveFromRoot();
	}

	Super::BeginDestroy();
}

bool UBLEManager::CheckBleSupport()
{
    UE_LOG(LogTemp, Warning, TEXT("BLEUTILS => UBLEManager::CheckBleSupport()"));
    
    return CentralManager.state != CBManagerStateUnsupported;
}

bool UBLEManager::IsBluetoothEnabled()
{
    UE_LOG(LogTemp, Warning, TEXT("BLEUTILS => UBLEManager::IsBluetoothEnabled()"));
    
    return CentralManager.state == CBManagerStatePoweredOn;
}

void UBLEManager::SetBluetoothState(bool enable)
{
	UE_LOG(LogTemp, Warning, TEXT("BLEUTILS => UBLEManager::SetBluetoothState()"));
	UE_LOG(LogTemp, Warning, TEXT("BLEUTILS => This functionality is not supported for iOS platform"));
}

void UBLEManager::ScanForDevices(const FOnDeviceFoundDelegate& onDeviceFoundCallback)
{
    UE_LOG(LogTemp, Warning, TEXT("BLEUTILS => UBLEManager::ScanForDevices()"));
    
    OnDeviceFoundDelegate = onDeviceFoundCallback;
    
    if(CentralManager.state == CBManagerStatePoweredOn)
    {
        UE_LOG(LogTemp, Warning, TEXT("BLEUTILS => Start scan..."));
        [CentralManager scanForPeripheralsWithServices:@[[CBUUID UUIDWithString:@"1818"]] options:@{CBCentralManagerScanOptionAllowDuplicatesKey : @NO}];
    }
}

void UBLEManager::StopScan()
{
     UE_LOG(LogTemp, Warning, TEXT("BLEUTILS => UBLEManager::StopScan()"));
    
    [CentralManager stopScan];
}

void UBLEManager::ConnectToDevice(TScriptInterface<IBLEDeviceInterface> device,
    const FOnDeviceConnectedDelegate& onDeviceConnectedCallback, const FOnConnectionErrorDelegate& onFailToConnectDeviceCallback)
{
    UE_LOG(LogTemp, Warning, TEXT("BLEUTILS => UBLEManager::ConnectToDevice()"));
    
    OnDeviceConnectedDelegate = onDeviceConnectedCallback;
    OnConnectionErrorDelegate = onFailToConnectDeviceCallback;
    
    UBLEDevice* BLEDevice = dynamic_cast<UBLEDevice*>(device.GetObject());
    
    if(!BLEDevice)
    {
        UE_LOG(LogTemp, Warning, TEXT("BLEUTILS => Failed to cast BLE device pointer"));
        return;
    }

    [CentralManager connectPeripheral:BLEDevice->GetDeviceInternal() options:nil];
}

void UBLEManager::DisconnectDevice(TScriptInterface<IBLEDeviceInterface> device,
    const FOnDeviceDisconnectedDelegate& onDeviceDisconnectedCallback, const FOnDisconnectionErrorDelegate& onFailToDosconnectDeviceCallback)
{
    UE_LOG(LogTemp, Warning, TEXT("BLEUTILS => UBLEManager::DisconnectDevice()"));
    
    OnDeviceDisconnectedDelegate = onDeviceDisconnectedCallback;
    OnDisconnectionErrorDelegate = onFailToDosconnectDeviceCallback;
    
    UBLEDevice* BLEDevice = dynamic_cast<UBLEDevice*>(device.GetObject());
    
    if(!BLEDevice)
    {
        UE_LOG(LogTemp, Warning, TEXT("BLEUTILS => Failed to cast BLE device pointer"));
        return;
    }
    
    [CentralManager cancelPeripheralConnection:BLEDevice->GetDeviceInternal()];
}
