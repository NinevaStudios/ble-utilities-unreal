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
