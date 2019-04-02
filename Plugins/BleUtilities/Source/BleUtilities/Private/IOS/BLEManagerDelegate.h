// Copyright (c) 2019 Nineva Studios

#pragma once

#import <Foundation/Foundation.h>
#import <CoreBluetooth/CoreBluetooth.h>

@interface BLEManagerDelegate : NSObject<CBCentralManagerDelegate>

@property(nonatomic, copy) void (^onDeviceFound)(CBPeripheral* device);

@property(nonatomic, copy) void (^onDeviceConnected)();
@property(nonatomic, copy) void (^onFailToConnect)(NSError* error);

@property(nonatomic, copy) void (^onDeviceDisconnected)();
@property(nonatomic, copy) void (^onFailToDisconnect)(NSError* error);

@end
