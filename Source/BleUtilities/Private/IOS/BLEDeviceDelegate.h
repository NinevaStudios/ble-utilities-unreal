// Copyright (c) 2019 Nineva Studios

#pragma once

#import <Foundation/Foundation.h>
#import <CoreBluetooth/CoreBluetooth.h>

enum class BLEDeviceError : uint8
{
	General,
	Read,
	Write,
	Notify
};

@interface BLEDeviceDelegate : NSObject<CBPeripheralDelegate>

@property(nonatomic, copy) void (^onWriteSuccess)(FString uuid);
@property(nonatomic, copy) void (^onValueUpdate)(FString uuid, NSData* data);
@property(nonatomic, copy) void (^onStopNotifications)(FString uuid);
@property(nonatomic, copy) void (^onError)(BLEDeviceError type, FString uuid, FString error);

@end
