// Copyright (c) 2019 Nineva Studios

#include "BLEDeviceDelegate.h"

#include "Async/Async.h"
#include "BleLog.h"
#include "IOS/BLEManager.h"

@implementation BLEDeviceDelegate{}

- (void)peripheral:(CBPeripheral *)peripheral didDiscoverServices:(NSError *)error {
	if (error) {
		UE_LOG(LogBle, Error, TEXT("Failed to discover the service"));
		const FString errorMessage([error localizedDescription]);
		AsyncTask(ENamedThreads::GameThread, [=]() {
			_onError(BLEDeviceError::General, "", errorMessage);
		});
		return;
	}

	if ([peripheral.services count] > 1) {
		UE_LOG(LogBle, Warning, TEXT("Device contains duplicate services."));
	}

	for (CBService* service in peripheral.services) {
		[peripheral discoverCharacteristics:nil forService:service];
	}
}

- (void)peripheral:(CBPeripheral *)peripheral didDiscoverCharacteristicsForService:(CBService *)service error:(NSError *)error {
	if (error) {
		UE_LOG(LogBle, Error, TEXT("Failed to subscribe for the service notifications"));
		const FString errorMessage([error localizedDescription]);
		AsyncTask(ENamedThreads::GameThread, [=]() {
			_onError(BLEDeviceError::General, "", errorMessage);
		});
		return;
	}
}

- (void)peripheral:(CBPeripheral *)peripheral didWriteValueForCharacteristic:(CBCharacteristic *)characteristic error:(NSError *)error {
	const FString charUUID(characteristic.UUID.UUIDString);

	if (error) {
		UE_LOG(LogBle, Error, TEXT("Failed to write value for %s"), *charUUID);
		const FString errorMessage([error localizedDescription]);
		AsyncTask(ENamedThreads::GameThread, [=]() {
			_onError(BLEDeviceError::Write, charUUID, errorMessage);
		});
		return;
	}

	UE_LOG(LogBle, Verbose, TEXT("Write value for %s"), *charUUID);
	AsyncTask(ENamedThreads::GameThread, [=]() {
		_onWriteSuccess(charUUID);
	});
}

- (void)peripheral:(CBPeripheral *)peripheral didUpdateValueForCharacteristic:(CBCharacteristic *)characteristic error:(NSError *)error {
	const FString charUUID(characteristic.UUID.UUIDString);

	if (error) {
		UE_LOG(LogBle, Error, TEXT("Failed to get value for %s"), *charUUID);
		const FString errorMessage([error localizedDescription]);
		AsyncTask(ENamedThreads::GameThread, [=]() {
			_onError(BLEDeviceError::Read, charUUID, errorMessage);
		});
		return;
	}

	UE_LOG(LogBle, Verbose, TEXT("Received value for %s"), *charUUID);
	AsyncTask(ENamedThreads::GameThread, [=]() {
		_onValueUpdate(charUUID, characteristic.value);
	});
}

- (void)peripheral:(CBPeripheral *)peripheral didUpdateNotificationStateForCharacteristic:(CBCharacteristic *)characteristic error:(NSError *)error {
	const FString charUUID(characteristic.UUID.UUIDString);

	if (error) {
		UE_LOG(LogBle, Error, TEXT("Failed to update notification state for %s"), *charUUID);
		const FString errorMessage([error localizedDescription]);
		AsyncTask(ENamedThreads::GameThread, [=]() {
			_onError(BLEDeviceError::Notify, charUUID, errorMessage);
		});
		return;
	}

	if (!characteristic.isNotifying) {
		UE_LOG(LogBle, Verbose, TEXT(" Notifications for %s stopped"), *charUUID);
		AsyncTask(ENamedThreads::GameThread, [=]() {
			_onStopNotifications(charUUID);
		});
		return;
	}
}

@end
