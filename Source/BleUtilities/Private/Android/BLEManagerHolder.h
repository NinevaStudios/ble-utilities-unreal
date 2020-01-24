// Copyright (c) 2019 Nineva Studios

#pragma once

#include "Interface/BLEManagerInterface.h"

class BLEManagerHolder
{
	BLEManagerHolder();

public:

	static BLEManagerHolder* GetInstance();

	void AddManager(FGuid guid, IBLEManagerInterface* pManager);
	void RemoveManager(FGuid guid);

	IBLEManagerInterface* const GetManager(FGuid guid) const;

private:

	TMap<FGuid, IBLEManagerInterface*> BleManagers;
};