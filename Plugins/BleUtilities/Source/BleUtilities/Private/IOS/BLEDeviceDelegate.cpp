// Copyright (c) 2019 Nineva Studios

#import "BLEDeviceDelegate.h"

@implementation BLEDeviceDelegate{
}

- (void)peripheral:(CBPeripheral *)peripheral didDiscoverServices:(NSError *)error {
    if (error) {
        NSLog(@"BLEUTILS = > Failed to discover Cycling Power service");
        _onError(error);
        return;
    }
    
    for (CBService *service in peripheral.services) {
        [peripheral discoverCharacteristics:@[[CBUUID UUIDWithString:@"2A63"]] forService:service];
    }
}

- (void)peripheral:(CBPeripheral *)peripheral didDiscoverCharacteristicsForService:(CBService *)service error:(NSError *)error {
    if (error) {
        NSLog(@"BLEUTILS = > Failed to subscribe for Cycling Power service notifications");
        _onError(error);
        return;
    }
    
    for (CBCharacteristic *characteristic in service.characteristics) {
        if ([characteristic.UUID isEqual:[CBUUID UUIDWithString:@"2A63"]]) {
            [peripheral setNotifyValue:YES forCharacteristic:characteristic];
        }
    }
}

- (void)peripheral:(CBPeripheral *)peripheral didUpdateValueForCharacteristic:(CBCharacteristic *)characteristic error:(NSError *)error {
    if (error) {
        NSLog(@"BLEUTILS = > Failed to receive update from Cycling Power service");
        _onError(error);
        return;
    }
    
    NSLog(@"BLEUTILS = > Received update from Cycling Power service");
    _onValueUpdate(characteristic.value);
}

- (void)peripheral:(CBPeripheral *)peripheral didUpdateNotificationStateForCharacteristic:(CBCharacteristic *)characteristic error:(NSError *)error {
    if (error) {
        NSLog(@"BLEUTILS = > Failed to stop notifications from Cycling Power service");
        _onError(error);
        return;
    }
    
    if(!characteristic.isNotifying) {
        NSLog(@"BLEUTILS = > Notifications from Cycling Power service stopped");
        _onStopNotifications();
        return;
    }
    
    NSLog(@"BLEUTILS = > Notifications from Cycling Power still coming...");
}

@end
