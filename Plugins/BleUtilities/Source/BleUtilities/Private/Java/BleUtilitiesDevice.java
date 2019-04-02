package com.ninevastudios.androidbleunrealapi;

import android.app.Activity;
import android.bluetooth.BluetoothDevice;
import android.bluetooth.BluetoothGatt;
import android.bluetooth.BluetoothGattCallback;
import android.bluetooth.BluetoothGattCharacteristic;
import android.bluetooth.BluetoothGattDescriptor;
import android.bluetooth.BluetoothGattService;
import android.support.annotation.Keep;
import android.util.Log;

import java.util.UUID;

@Keep
public class BleUtilitiesDevice {

	private static final UUID CYCLING_POWER_MEASUREMENT_UUID = convertFromInteger(0x2A63);
	private static final UUID CLIENT_CHARACTERISTIC_CONFIGURATION_UUID = convertFromInteger(0x2902);
	private static final UUID CYCLING_POWER_SERVICE_UUID = convertFromInteger(0x1818);

	private Activity _activity;
	private BluetoothDevice _device;
	private String _id;
	private String _managerId;
	private BluetoothGatt _bluetoothGatt;
	private BluetoothGattCharacteristic _bluetoothCharacteristic;
	private int _state = BluetoothGatt.STATE_DISCONNECTED;

	@Keep
	public static native void onDeviceConnected(String guid);
	@Keep
	public static native void onDeviceDisconnected(String guid);
	@Keep
	public static native void onNotificationReceived(String guid, byte[] bytes);
	@Keep
	public static native void onUnsubscribed(String guid);
	@Keep
	public static native void onNotificationError(String guid, String error);

	@Keep
	public BleUtilitiesDevice(Activity activity, BluetoothDevice device) {
		_activity = activity;
		_device = device;
	}

	@Keep
	public void setObjectId(String id) {
		_id = id;
	}

	@Keep
	public String getDeviceName() {
		Log.w("OSBLE => ", "getDeviceName: getDeviceName");
		return _device.getName();
	}

	@Keep
	public String getDeviceId(){
		return _device.getAddress();
	}

	@Keep
	public void connect(String managerId) {
		_managerId = managerId;
		_bluetoothGatt = _device.connectGatt(_activity, false, getGattCallback(managerId));
	}

	@Keep
	public void disconnect() {
		if (_bluetoothGatt == null) {
			return;
		}
		_bluetoothGatt.close();
		_bluetoothGatt.disconnect();
		_bluetoothGatt = null;

		_state = BluetoothGatt.STATE_DISCONNECTED;

		onDeviceDisconnected(_managerId);
	}

	@Keep
	public boolean isConnected() {
		return _state == BluetoothGatt.STATE_CONNECTED;
	}

	@Keep
	public void subscribe() {
		_activity.runOnUiThread(new Runnable() {
			@Override
			public void run() {
			if (_state == BluetoothGatt.STATE_CONNECTED) {
				_bluetoothGatt.discoverServices();
			}
			}
		});
	}

	@Keep
	public void unsubscribe() {
		_activity.runOnUiThread(new Runnable() {
			@Override
			public void run() {
				if (_state == BluetoothGatt.STATE_CONNECTED) {
					if (_bluetoothCharacteristic != null) {
						_bluetoothGatt.setCharacteristicNotification(_bluetoothCharacteristic, false);
						onUnsubscribed(_id);
						BluetoothGattDescriptor descriptor = _bluetoothCharacteristic.getDescriptor(CLIENT_CHARACTERISTIC_CONFIGURATION_UUID);
						if (descriptor != null) {
							descriptor.setValue(BluetoothGattDescriptor.DISABLE_NOTIFICATION_VALUE);
							_bluetoothGatt.writeDescriptor(descriptor);
						} else {
							onNotificationError(_id, "Could not stop notification subscription");
						}
					}
					else {
						onNotificationError(_id, "Could not get characteristic");
					}
				}
			}
		});
	}

	@Keep
	private BluetoothGattCallback getGattCallback(final String managerId) {
		return new BluetoothGattCallback() {
			@Override
			public void onConnectionStateChange(final BluetoothGatt gatt, int status, final int newState) {
				_activity.runOnUiThread(new Runnable() {
					@Override
					public void run() {
						if ((newState == BluetoothGatt.STATE_CONNECTED) && (_state != BluetoothGatt.STATE_CONNECTED)) {
							_state = newState;
							onDeviceConnected(managerId);
						}
					}
				});
			}

			@Override
			public void onServicesDiscovered(final BluetoothGatt gatt, final int status) {
				_activity.runOnUiThread(new Runnable() {
					@Override
					public void run() {
						if (status == BluetoothGatt.GATT_SUCCESS) {
							BluetoothGattService service = gatt.getService(CYCLING_POWER_SERVICE_UUID);
							if (service != null) {
								_bluetoothCharacteristic = service.getCharacteristic(CYCLING_POWER_MEASUREMENT_UUID);
								if (_bluetoothCharacteristic != null) {
									gatt.setCharacteristicNotification(_bluetoothCharacteristic, true);

									BluetoothGattDescriptor descriptor = _bluetoothCharacteristic.getDescriptor(CLIENT_CHARACTERISTIC_CONFIGURATION_UUID);
									if (descriptor != null) {
										descriptor.setValue(BluetoothGattDescriptor.ENABLE_NOTIFICATION_VALUE);
										gatt.writeDescriptor(descriptor);
									} else {
										onNotificationError(_id,"Could not get the Client Characteristic Configuration descriptor");
									}
								} else {
									onNotificationError(_id,"Could not get the characteristic from the service");
								}
							} else {
								onNotificationError(_id,"Could not get the service");
							}
						} else {
							onNotificationError(_id,"GATT operation was not successful");
						}
					}
				});
			}

			@Override
			public void onCharacteristicRead(BluetoothGatt gatt, final BluetoothGattCharacteristic characteristic,
											 final int status) {
				_activity.runOnUiThread(new Runnable() {
					@Override
					public void run() {
						if (status == BluetoothGatt.GATT_SUCCESS) {
							Log.d("ANDROIDBLE", "Characteristic read success.");
							if (characteristic.getUuid().equals(CYCLING_POWER_MEASUREMENT_UUID)) {
								Log.d("ANDROIDBLE", "UUID match.");
								onNotificationReceived(_id, characteristic.getValue());
							}
						} else {
							onNotificationError(_id,"GATT operation was not successful");
						}
					}
				});
			}

			@Override
			public void onCharacteristicChanged(BluetoothGatt gatt, final BluetoothGattCharacteristic characteristic) {
				_activity.runOnUiThread(new Runnable() {
					@Override
					public void run() {
						onNotificationReceived(_id, characteristic.getValue());
					}
				});
			}
		};
	}

	private static UUID convertFromInteger(int i) {
		final long MSB = 0x0000000000001000L;
		final long LSB = 0x800000805f9b34fbL;
		return new UUID(MSB | ((long) i << 32), LSB);
	}
}
