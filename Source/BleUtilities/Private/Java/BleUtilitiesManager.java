// Copyright (c) 2020 Nineva Studios

package com.ninevastudios.androidbleunrealapi;

import android.app.Activity;
import android.bluetooth.BluetoothAdapter;
import android.bluetooth.BluetoothDevice;
import android.content.pm.PackageManager;
import android.os.Handler;
import android.os.Looper;
import android.support.annotation.Keep;

import java.util.HashMap;
import java.util.Iterator;
import java.util.Map;
import java.util.UUID;

@Keep
public class BleUtilitiesManager {

	private static long SCAN_PERIOD = 30000;

	private BluetoothAdapter mBluetoothAdapter = BluetoothAdapter.getDefaultAdapter();
	private BluetoothAdapter.LeScanCallback mLeScanCallback;
	private Handler handler = new Handler(Looper.getMainLooper());

	private Map<String, BleUtilitiesDevice> devicesMap = new HashMap<>();

	private Activity _activity;
	private String _id;
	private String _serviceUuid;

	@Keep
	public static native void onNewDeviceDiscovered(String guid, BleUtilitiesDevice device, String serviceUuid);

	@Keep
	public BleUtilitiesManager(Activity activity, String guid, String serviceUuid) {
		_activity = activity;
		_id = guid;
		_serviceUuid = serviceUuid;
	}

	@Keep
	public boolean isBluetoothEnabled() {
		if (mBluetoothAdapter == null) {
			return false;
		} else {
			return mBluetoothAdapter.isEnabled();
		}
	}

	@Keep
	public boolean supportsBle() {
		return _activity.getPackageManager().hasSystemFeature(PackageManager.FEATURE_BLUETOOTH_LE);
	}

	@Keep
	public void setBluetoothEnabled(boolean isEnabled) {
		if (isEnabled) {
			mBluetoothAdapter.enable();
		} else {
			mBluetoothAdapter.disable();
		}
	}

	@Keep
	public void startScan() {
		for (Iterator<Map.Entry<String, BleUtilitiesDevice>> it = devicesMap.entrySet().iterator(); it.hasNext();) {
			Map.Entry<String, BleUtilitiesDevice> deviceEntry = it.next();
			if (!deviceEntry.getValue().isConnected()) {
				it.remove();
			}
		}

		mLeScanCallback = getLeScanCallback();
		mBluetoothAdapter.startLeScan(new UUID[]{UUID.fromString(_serviceUuid)}, mLeScanCallback);
	}

	@Keep
	public void stopScan() {
		mBluetoothAdapter.stopLeScan(mLeScanCallback);
	}

	@Keep
	public void connect(String deviceId) {
		devicesMap.get(deviceId).connect(_id);
	}

	@Keep
	public void disconnect(String deviceId) {
		devicesMap.get(deviceId).disconnect();
	}

	private BluetoothAdapter.LeScanCallback getLeScanCallback() {
		return new BluetoothAdapter.LeScanCallback() {
			@Override
			public void onLeScan(final BluetoothDevice device, final int rssi, byte[] scanRecord) {
				_activity.runOnUiThread(new Runnable() {
					@Override
					public void run() {
						if (!devicesMap.containsKey(device.getAddress())) {
							BleUtilitiesDevice dev = new BleUtilitiesDevice(_activity, device, _serviceUuid);
							devicesMap.put(device.getAddress(), dev);
							onNewDeviceDiscovered(_id, dev, _serviceUuid);
						}
					}
				});
			}
		};
	}
}
