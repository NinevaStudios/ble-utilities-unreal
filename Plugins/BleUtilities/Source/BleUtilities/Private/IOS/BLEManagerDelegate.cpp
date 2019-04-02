// Copyright (c) 2019 Nineva Studios

#import "BLEManagerDelegate.h"

@implementation BLEManagerDelegate{
}

-(void)centralManagerDidUpdateState:(CBCentralManager *)central {
	if (central.state == CBManagerStatePoweredOff) {
		NSLog(@"BLEUTILS = > Bluetooth OFF");
		return;
	}

	if (central.state == CBManagerStatePoweredOn) {
		NSLog(@"BLEUTILS = > Bluetooth ON");
	}
}

- (void)centralManager:(CBCentralManager *)central didDiscoverPeripheral:(CBPeripheral *)peripheral advertisementData:(NSDictionary<NSString *,id> *)advertisementData
                  RSSI:(NSNumber *)RSSI {
    NSLog(@"BLEUTILS = > Discovered peripheral %@ at %@", peripheral.name, RSSI);
    _onDeviceFound(peripheral);
}

- (void)centralManager:(CBCentralManager *)central didConnectPeripheral:(CBPeripheral *)peripheral {
    NSLog(@"BLEUTILS = > Connected to device successfully");
    _onDeviceConnected();
}

- (void)centralManager:(CBCentralManager *)central didFailToConnectPeripheral:(CBPeripheral *)peripheral error:(NSError *)error {
    NSLog(@"BLEUTILS = > Connection to device failed");
    _onFailToConnect(error);
}

- (void)centralManager:(CBCentralManager *)central didDisconnectPeripheral:(CBPeripheral *)peripheral error:(NSError *)error {
    if (error) {
        NSLog(@"BLEUTILS = > Device disconnection failed");
        _onFailToDisconnect(error);
        return;
    }
    
    NSLog(@"BLEUTILS = > Device disconnected successfully");
    _onDeviceDisconnected();
}

@end
