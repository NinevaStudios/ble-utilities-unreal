// Copyright (c) 2019 Nineva Studios

#include "BLEDevice.h"

#import "BLEDeviceDelegate.h"

UBLEDevice::UBLEDevice(const FObjectInitializer& ObjectInitializer)
    : Super(ObjectInitializer)
{
    UE_LOG(LogTemp, Warning, TEXT("BLEUTILS => UBLEDevice::UBLEManager()"));
}

UBLEDevice::~UBLEDevice()
{
    UE_LOG(LogTemp, Warning, TEXT("BLEUTILS => UBLEDevice::~UBLEDevice()"));
    
    if(Device)
    {
        Device = (CBPeripheral*)CFBridgingRelease(Device);
    }
}

void UBLEDevice::SetDeviceInternal(CBPeripheral* device)
{
    UE_LOG(LogTemp, Warning, TEXT("BLEUTILS => UBLEDevice::SetDeviceInternal()"));
    
    Device = (CBPeripheral*)CFBridgingRetain(device);
    
    BLEDeviceDelegate* bleDeviceDelegate = [[BLEDeviceDelegate alloc] init];

    bleDeviceDelegate.onValueUpdate = ^(NSData* data) {
        TArray<uint8> byteData;
        uint8* byteArray = (uint8*)data.bytes;
        for(int i = 0; i < data.length; i++)
        {
            byteData.Add(byteArray[i]);
        }

        OnNotificationReceivedDelegate.ExecuteIfBound(byteData);
    };
    bleDeviceDelegate.onStopNotifications = ^() {
        OnNotificationsStoppedDelegate.ExecuteIfBound();
    };
    bleDeviceDelegate.onError = ^(NSError* error) {
        FString errorSrt(error.localizedDescription);
        OnNotificationErrorDelegate.ExecuteIfBound(errorSrt);
    };

    Device.delegate = bleDeviceDelegate;
}

CBPeripheral* UBLEDevice::GetDeviceInternal()
{
    UE_LOG(LogTemp, Warning, TEXT("BLEUTILS => UBLEDevice::GetDeviceInternal()"));
    
    return Device;
}

FString UBLEDevice::GetDeviceName()
{
    UE_LOG(LogTemp, Warning, TEXT("BLEUTILS => UBLEDevice::GetDeviceName()"));
    
    return FString(Device.name);
}

FString UBLEDevice::GetDeviceId()
{
    UE_LOG(LogTemp, Warning, TEXT("BLEUTILS => UBLEDevice::GetDeviceId()"));
    
    return FString([Device.identifier UUIDString]);
}

bool UBLEDevice::IsConnected()
{
    UE_LOG(LogTemp, Warning, TEXT("BLEUTILS => UBLEDevice::IsConnected()"));
    
    return Device.state == CBPeripheralStateConnected;
}

void UBLEDevice::SubscribeForNotifications(const FOnNotificationReceivedDelegate& onNotificationReceivedCallback,
    const FOnNotificationErrorDelegate& onNotificationErrorCallback)
{
    UE_LOG(LogTemp, Warning, TEXT("BLEUTILS => UBLEDevice::SubscribeForNotifications()"));
    
    OnNotificationReceivedDelegate = onNotificationReceivedCallback;
    OnNotificationErrorDelegate = onNotificationErrorCallback;
    
    [Device discoverServices:@[[CBUUID UUIDWithString:@"1818"]]];
}

void UBLEDevice::StopNotifications(const FOnNotificationsStoppedDelegate& onNotificationsStoppedCallback,
    const FOnNotificationErrorDelegate& onNotificationErrorCallback)
{
    UE_LOG(LogTemp, Warning, TEXT("BLEUTILS => UBLEDevice::StopNotifications()"));
    
    OnNotificationsStoppedDelegate = onNotificationsStoppedCallback;
    OnNotificationErrorDelegate = onNotificationErrorCallback;
    
    if(Device.services != nil) {
        for (CBService *service in Device.services) {
            if(service.characteristics != nil) {
                for (CBCharacteristic *characteristic in service.characteristics) {
                    if ([characteristic.UUID isEqual:[CBUUID UUIDWithString:@"2A63"]]) {
                        [Device setNotifyValue:NO forCharacteristic:characteristic];
                    }
                }
            }
        }
    }
}
