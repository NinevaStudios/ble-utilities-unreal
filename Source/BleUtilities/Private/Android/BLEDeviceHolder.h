// Copyright (c) 2019 Nineva Studios

#pragma once

#include "Interface/BLEDeviceInterface.h"

using DeviceId = TPair<FString, FString>;

class BLEDeviceHolder
{
	BLEDeviceHolder();

public:

	static BLEDeviceHolder* GetInstance();

	void AddDevice(DeviceId id, IBLEDeviceInterface* pDevice);
	void RemoveDevice(DeviceId id);

	IBLEDeviceInterface* const GetDevice(DeviceId id) const;

private:

	TMap<DeviceId, IBLEDeviceInterface*> BleDevices;
};