// Copyright (c) 2019 Nineva Studios

#include "BLEManagerDelegate.h"

#include "Async/Async.h"
#include "BleLog.h"

@implementation BLEManagerDelegate{}

- (void)centralManagerDidUpdateState:(CBCentralManager *)central {}

- (void)centralManager:(CBCentralManager *)central didDiscoverPeripheral:(CBPeripheral *)peripheral
						advertisementData:(NSDictionary<NSString *,id> *)advertisementData RSSI:(NSNumber *)RSSI {
	CBPeripheral* retainedPeripheral = (CBPeripheral*)CFBridgingRetain(peripheral);
	AsyncTask(ENamedThreads::GameThread, [=]() {
		_onDeviceFound(retainedPeripheral, central);
	});
}

- (void)centralManager:(CBCentralManager *)central didConnectPeripheral:(CBPeripheral *)peripheral {
	UE_LOG(LogBle, Verbose, TEXT("Connected to device successfully"));
	CBPeripheral* retainedPeripheral = (CBPeripheral*)CFBridgingRetain(peripheral);
	AsyncTask(ENamedThreads::GameThread, [=]() {
		_onDeviceConnected(retainedPeripheral, central);
	});
}

- (void)centralManager:(CBCentralManager *)central didFailToConnectPeripheral:(CBPeripheral *)peripheral error:(NSError *)error {
	UE_LOG(LogBle, Error, TEXT("Connection to device failed"));
	const FString errorMessage([error localizedDescription]);
	AsyncTask(ENamedThreads::GameThread, [=]() {
		_onFailToConnect(errorMessage);
	});
}

- (void)centralManager:(CBCentralManager *)central didDisconnectPeripheral:(CBPeripheral *)peripheral error:(NSError *)error {
	if (error) {
		UE_LOG(LogBle, Error, TEXT("Device disconnect failed"));
		const FString errorMessage([error localizedDescription]);
		AsyncTask(ENamedThreads::GameThread, [=]() {
			_onFailToDisconnect(errorMessage);
		});
		return;
	}

	UE_LOG(LogBle, Verbose, TEXT("Device disconnected successfully"));
	AsyncTask(ENamedThreads::GameThread, [=]() {
		_onDeviceDisconnected();
	});
}

@end
