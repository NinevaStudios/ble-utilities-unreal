// Copyright (c) 2020 Nineva Studios

#include "BLEScannerDelegate.h"
#include "Async/Async.h"

@implementation BLEScannerDelegate {}

- (void)centralManagerDidUpdateState:(CBCentralManager *)central {}

- (void)centralManager:(CBCentralManager *)central didDiscoverPeripheral:(CBPeripheral *)peripheral
						advertisementData:(NSDictionary<NSString *,id> *)advertisementData RSSI:(NSNumber *)RSSI {
	if ([[self nameFilters] count] != 0 && [[self nameFilters] containsObject:peripheral.name] == NO)
	{
		return;
	}

	FString deviceName(peripheral.name);
	NSData* data = [[NSData alloc] initWithData:advertisementData[CBAdvertisementDataManufacturerDataKey]];

	AsyncTask(ENamedThreads::GameThread, [=]() {
		_onAdvertisementReceived(deviceName, data);
	});
}

@end
