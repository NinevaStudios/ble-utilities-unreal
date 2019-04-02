# Unreal Engine 4 BLE (Bluetooth Low Enegy) Plugin

This is Unreal Engine plugin that allows to scan for BLE devices with Cycling Power service running, connect to one of them and subscribe for its notifications.

This is what the plugin is able to do:
- Works in Unreal Engine 4.20 for both iOS and Android platforms
- Check if bluetooth Low Engergy is supported on the device
- Check if bluetooth is enabled
- Enable/disable bluetooth" (Android only)
- Scan for bluetooth devices that have the "Cycling Power" service running
- Connect to one of the found BLE devices from within the Unreal Engine app
- Subscribe to the "Cycling  Power" -> "Cycling Power Measurement" notifications
- Return the received notification data as byte array to Unreal Engine

Here are some blueprint screenshots:

1. Check bluetooth status

<img src="https://github.com/NinevaStudios/BleUtilitiesPluginUnreal/blob/master/Resource/ble1.PNG">

2. Scan for BLE devices

<img src="https://github.com/NinevaStudios/BleUtilitiesPluginUnreal/blob/master/Resource/ble2.PNG">

3. Subscribe for notifications

<img src="https://github.com/NinevaStudios/BleUtilitiesPluginUnreal/blob/master/Resource/ble3.PNG">
