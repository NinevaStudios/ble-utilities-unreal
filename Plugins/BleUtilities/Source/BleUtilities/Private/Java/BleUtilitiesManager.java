package com.ninevastudios.androidbleunrealapi;

import android.app.Activity;
import android.bluetooth.BluetoothAdapter;
import android.bluetooth.BluetoothDevice;
import android.content.pm.PackageManager;
import android.os.Handler;
import android.os.Looper;
import android.support.annotation.Keep;

import java.util.HashMap;
import java.util.Map;
import java.util.UUID;

@Keep
public class BleUtilitiesManager {

	private static final UUID CYCLING_POWER_SERVICE_UUID = convertFromInteger(0x1818);
	private static long SCAN_PERIOD = 30000;

	private BluetoothAdapter mBluetoothAdapter = BluetoothAdapter.getDefaultAdapter();
	private BluetoothAdapter.LeScanCallback mLeScanCallback = getLeScanCallback();
	private Handler handler = new Handler(Looper.getMainLooper());

	private Map<String, BleUtilitiesDevice> devicesMap = new HashMap<>();

	private Activity _activity;
	private String _id;

	@Keep
	public static native void onNewDeviceDiscovered(String guid, String deviceId);

	@Keep
	public BleUtilitiesManager(Activity activity, String guid) {
		_activity = activity;
		_id = guid;
	}

	@Keep
	public void setDeviceId(String id, String guid) {
		devicesMap.get(id).setObjectId(guid);
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
		devicesMap.clear();
		mBluetoothAdapter.startLeScan(new UUID[]{BleUtilitiesManager.CYCLING_POWER_SERVICE_UUID}, mLeScanCallback);
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
	public boolean isConnected(String deviceId) {
		return devicesMap.get(deviceId).isConnected();
	}

	@Keep
	public void disconnect(String deviceId) {
		devicesMap.get(deviceId).disconnect();
	}

	@Keep
	public void subscribe(String deviceId) {
		devicesMap.get(deviceId).subscribe();
	}

	@Keep
	public void unsubscribe(String deviceId) {
		devicesMap.get(deviceId).unsubscribe();
	}

	@Keep
	public String getDeviceName(String deviceId)
	{
		return devicesMap.get(deviceId).getDeviceName();
	}

	private BluetoothAdapter.LeScanCallback getLeScanCallback() {
		return new BluetoothAdapter.LeScanCallback() {
			@Override
			public void onLeScan(final BluetoothDevice device, final int rssi, byte[] scanRecord) {
				_activity.runOnUiThread(new Runnable() {
					@Override
					public void run() {
						if (!devicesMap.containsKey(device.getAddress())) {
							devicesMap.put(device.getAddress(), new BleUtilitiesDevice(_activity, device));
							onNewDeviceDiscovered(_id, device.getAddress());
						}
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
