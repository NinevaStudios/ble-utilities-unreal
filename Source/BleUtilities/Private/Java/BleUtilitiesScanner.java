// Copyright (c) 2020 Nineva Studios

package com.ninevastudios.androidbleunrealapi;

import android.app.Activity;
import android.bluetooth.BluetoothAdapter;
import android.bluetooth.BluetoothDevice;
import android.bluetooth.le.BluetoothLeScanner;
import android.bluetooth.le.ScanCallback;
import android.bluetooth.le.ScanFilter;
import android.bluetooth.le.ScanRecord;
import android.bluetooth.le.ScanResult;
import android.bluetooth.le.ScanSettings;
import android.support.annotation.Keep;
import android.util.Log;
import android.util.SparseArray;

import java.util.ArrayList;
import java.util.List;

@Keep
public class BleUtilitiesScanner {

	private Activity _activity;
	private long _callbackAddr;

	private BluetoothLeScanner _scanner = BluetoothAdapter.getDefaultAdapter().getBluetoothLeScanner();

	@Keep
	public static native void onAdvertisementReceived(long callbackAddr, String deviceName, byte[] bytes);

	@Keep
	public BleUtilitiesScanner(Activity activity, long callbackAddr) {
		_activity = activity;
		_callbackAddr = callbackAddr;
	}

	private ScanCallback _scanCallback = new ScanCallback() {
		@Override
		public void onScanResult(int callbackType, final ScanResult result) {
			_activity.runOnUiThread(new Runnable() {
				@Override
				public void run() {
					ScanRecord record = result.getScanRecord();

					Log.i("BLESCAN:", "Scan Record: " +  record.toString());
					SparseArray<byte[]> manufacturerData = record.getManufacturerSpecificData();
					for(int i = 0; i < manufacturerData.size(); i++){
						onAdvertisementReceived(_callbackAddr, record.getDeviceName(), manufacturerData.valueAt(i));
					}
				}
			});
		}
		@Override
		public void onScanFailed(int errorCode) {
		}
	};

	@Keep
	public void startScan(String[] nameFilters) {
		try {
			if (nameFilters.length == 0) {
				_scanner.startScan(_scanCallback);
			} else {
				List<ScanFilter> filters = new ArrayList<>();
				for (String nameFilter : nameFilters) {
					filters.add(new ScanFilter.Builder().setDeviceName(nameFilter).build());
				}

				ScanSettings settings = new ScanSettings.Builder().setScanMode(ScanSettings.SCAN_MODE_LOW_LATENCY).build();
				_scanner.startScan(filters, settings, _scanCallback);
			}
		}
		catch (Exception e) {
			Log.i("BLESCAN:", "Failed to start scan: " +  e.toString());
		}
	}

	@Keep
	public void stopScan() {
		_scanner.stopScan(_scanCallback);
	}
}
