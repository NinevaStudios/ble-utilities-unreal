// Copyright (c) 2019 Nineva Studios

#pragma once

#import <Foundation/Foundation.h>
#import <CoreBluetooth/CoreBluetooth.h>

@interface CustomCBCentralManager : CBCentralManager

@property(nonatomic) FString serviceUUID;

@end
