// Copyright (c) 2019 Nineva Studios

#pragma once

#import <Foundation/Foundation.h>
#import <CoreBluetooth/CoreBluetooth.h>

@interface BLEDeviceDelegate : NSObject<CBPeripheralDelegate>

@property(nonatomic, copy) void (^onValueUpdate)(NSData* data);
@property(nonatomic, copy) void (^onStopNotifications)();
@property(nonatomic, copy) void (^onError)(NSError* error);

@end
