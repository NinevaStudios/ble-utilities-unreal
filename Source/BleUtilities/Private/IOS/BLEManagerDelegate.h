// Copyright (c) 2019 Nineva Studios

#pragma once

#import <Foundation/Foundation.h>
#import <CoreBluetooth/CoreBluetooth.h>

@interface BLEManagerDelegate : NSObject<CBCentralManagerDelegate>

@property(nonatomic, copy) void (^onDeviceFound)(CBPeripheral* device, CBCentralManager* manager);

@property(nonatomic, copy) void (^onDeviceConnected)(CBPeripheral* device, CBCentralManager* manager);
@property(nonatomic, copy) void (^onFailToConnect)(FString error);

@property(nonatomic, copy) void (^onDeviceDisconnected)();
@property(nonatomic, copy) void (^onFailToDisconnect)(FString error);

@end
