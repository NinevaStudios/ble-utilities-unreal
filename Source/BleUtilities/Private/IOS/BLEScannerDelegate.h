// Copyright (c) 2020 Nineva Studios

#pragma once

#import <Foundation/Foundation.h>
#import <CoreBluetooth/CoreBluetooth.h>

@interface BLEScannerDelegate : NSObject<CBCentralManagerDelegate>

@property(nonatomic, copy) void (^onAdvertisementReceived)(FString deviceName, NSData* data);
@property(nonatomic, copy) NSMutableArray<NSString*>* nameFilters;

@end
