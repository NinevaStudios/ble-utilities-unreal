// Copyright (c) 2019 Nineva Studios

#pragma once

#include "Interface/BLEDeviceInterface.h"

class BLEDeviceHolder
{
	BLEDeviceHolder();

public:

	static BLEDeviceHolder* GetInstance();

	void AddDevice(FGuid guid, IBLEDeviceInterface* pDevice);
	void RemoveDevice(FGuid guid);

	IBLEDeviceInterface* GetDevice(FGuid guid) const;

private:

	TMap<FGuid, IBLEDeviceInterface*> BleDevices;
};