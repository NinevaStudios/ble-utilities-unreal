// Copyright (c) 2020 Nineva Studios

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

	private Activity _activity;
	private BluetoothDevice _device;
	private String _id;
	private String _managerId;
	private BluetoothGatt _bluetoothGatt;
	private BluetoothGattService _service;
	private int _state = BluetoothGatt.STATE_DISCONNECTED;
	private UUID _serviceUuid;

	@Keep
	public static native void onDeviceConnected(String guid);
	@Keep
	public static native void onDeviceDisconnected(String guid);
	@Keep
	public static native void onReadSuccess(String deviceUuid, String serviceUuid, String characteristicUuid, byte[] bytes);
	@Keep
	public static native void onReadError(String deviceUuid, String serviceUuid, String characteristicUuid, String error);
	@Keep
	public static native void onWriteSuccess(String deviceUuid, String serviceUuid, String characteristicUuid);
	@Keep
	public static native void onWriteError(String deviceUuid, String serviceUuid, String characteristicUuid, String error);
	@Keep
	public static native void onNotificationReceived(String deviceUuid, String serviceUuid, String characteristicUuid, byte[] bytes);
	@Keep
	public static native void onNotificationError(String deviceUuid, String serviceUuid, String characteristicUuid, String error);
	@Keep
	public static native void onUnsubscribed(String deviceUuid, String serviceUuid, String characteristicUuid);
	@Keep
	public static native void onUnsubscribedError(String deviceUuid, String serviceUuid, String characteristicUuid, String error);

	@Keep
	public BleUtilitiesDevice(Activity activity, BluetoothDevice device, String serviceUuid) {
		_activity = activity;
		_device = device;
		_serviceUuid = UUID.fromString(serviceUuid);
		_id = _device.getAddress();
	}

	@Keep
	public String getDeviceName() {
		return _device.getName();
	}

	@Keep
	public String getDeviceId(){
		return _id;
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
	public void discoverService() {
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
	public void readValue(final String characteristicUuidString) {
		_activity.runOnUiThread(new Runnable() {
			@Override
			public void run() {
				if (_state == BluetoothGatt.STATE_CONNECTED && _service != null) {
					UUID characteristicUuid = UUID.fromString(characteristicUuidString);
					BluetoothGattCharacteristic characteristic = _service.getCharacteristic(characteristicUuid);
					if (characteristic != null) {
						_bluetoothGatt.readCharacteristic(characteristic);
					} else {
						onReadError(_id, _serviceUuid.toString(), characteristicUuidString, "Could not get characteristic");
					}
				} else {
					onReadError(_id, _serviceUuid.toString(), characteristicUuidString, "Could not get service");
				}
			}
		});
	}

	@Keep
	public void writeValue(final String characteristicUuidString, final byte[] bytes) {
		_activity.runOnUiThread(new Runnable() {
			@Override
			public void run() {
				if (_state == BluetoothGatt.STATE_CONNECTED && _service != null) {
					UUID characteristicUuid = UUID.fromString(characteristicUuidString);
					BluetoothGattCharacteristic characteristic = _service.getCharacteristic(characteristicUuid);
					if (characteristic != null) {
						characteristic.setValue(bytes);
						_bluetoothGatt.writeCharacteristic(characteristic);
					} else {
						onWriteError(_id, _serviceUuid.toString(), characteristicUuidString, "Could not get characteristic");
					}
				} else {
					onWriteError(_id, _serviceUuid.toString(), characteristicUuidString, "Could not get service");
				}
			}
		});
	}

	@Keep
	public void subscribe(final String characteristicUuidString, final boolean isIndication) {
		_activity.runOnUiThread(new Runnable() {
			@Override
			public void run() {
				if (_state == BluetoothGatt.STATE_CONNECTED && _service != null) {
					UUID characteristicUuid = UUID.fromString(characteristicUuidString);
					BluetoothGattCharacteristic characteristic = _service.getCharacteristic(characteristicUuid);
					if (characteristic != null) {
						_bluetoothGatt.setCharacteristicNotification(characteristic, true);
						BluetoothGattDescriptor descriptor = characteristic.getDescriptor(clientCharacteristicConfigurationUUID());
						if (descriptor != null) {
							descriptor.setValue(isIndication ?
									BluetoothGattDescriptor.ENABLE_INDICATION_VALUE :
									BluetoothGattDescriptor.ENABLE_NOTIFICATION_VALUE);
							_bluetoothGatt.writeDescriptor(descriptor);
						} else {
							onNotificationError(_id, _serviceUuid.toString(), characteristicUuidString,"Could not get the Client Characteristic Configuration descriptor");
						}
					} else {
						onNotificationError(_id, _serviceUuid.toString(), characteristicUuidString,"Could not get characteristic");
					}
				} else {
					onNotificationError(_id, _serviceUuid.toString(), characteristicUuidString, "Could not get service");
				}
			}
		});
	}

	@Keep
	public void unsubscribe(final String characteristicUuidString) {
		_activity.runOnUiThread(new Runnable() {
			@Override
			public void run() {
				if (_state == BluetoothGatt.STATE_CONNECTED && _service != null) {
					UUID characteristicUuid = UUID.fromString(characteristicUuidString);
					BluetoothGattCharacteristic characteristic = _service.getCharacteristic(characteristicUuid);
					if (characteristic != null) {
						_bluetoothGatt.setCharacteristicNotification(characteristic, false);
						onUnsubscribed(_id, _serviceUuid.toString(), characteristicUuidString);
						BluetoothGattDescriptor descriptor = characteristic.getDescriptor(clientCharacteristicConfigurationUUID());
						if (descriptor != null) {
							descriptor.setValue(BluetoothGattDescriptor.DISABLE_NOTIFICATION_VALUE);
							_bluetoothGatt.writeDescriptor(descriptor);
						} else {
							onUnsubscribedError(_id, _serviceUuid.toString(), characteristicUuidString, "Could not stop notification subscription for characteristic");
						}
					} else {
						onUnsubscribedError(_id, _serviceUuid.toString(), characteristicUuidString, "Could not get characteristic");
					}
				} else {
					onUnsubscribedError(_id, _serviceUuid.toString(), characteristicUuidString, "Could not get service");
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
							discoverService();
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
							BluetoothGattService service = gatt.getService(_serviceUuid);
							if (service != null) {
								_service = service;
								Log.i("BLE => ", "Found service " + _serviceUuid);
							}
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
							onReadSuccess(_id, _serviceUuid.toString(), characteristic.getUuid().toString(), characteristic.getValue());
						}
						else {
							onReadError(_id, _serviceUuid.toString(), characteristic.getUuid().toString(),"Characteristic read failed. Error code: " + status);
						}
					}
				});
			}

			@Override
			public void onCharacteristicWrite(BluetoothGatt gatt, final BluetoothGattCharacteristic characteristic,
											  final int status) {
				_activity.runOnUiThread(new Runnable() {
					@Override
					public void run() {
						if (status == BluetoothGatt.GATT_SUCCESS) {
							onWriteSuccess(_id, _serviceUuid.toString(), characteristic.getUuid().toString());
						}
						else {
							onWriteError(_id, _serviceUuid.toString(), characteristic.getUuid().toString(), "Characteristic write failed. Error code: " + status);
						}
					}
				});
			}

			@Override
			public void onCharacteristicChanged(BluetoothGatt gatt, final BluetoothGattCharacteristic characteristic) {
				_activity.runOnUiThread(new Runnable() {
					@Override
					public void run() {
						onNotificationReceived(_id, _serviceUuid.toString(), characteristic.getUuid().toString(), characteristic.getValue());
					}
				});
			}
		};
	}

	private static UUID clientCharacteristicConfigurationUUID()
	{
		return BleUtils.intToUUID(0x2902);
	}
}
