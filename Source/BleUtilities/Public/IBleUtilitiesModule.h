// Copyright (c) 2019 Nineva Studios

#pragma once

#include "Modules/ModuleManager.h"

class IBleUtilitiesModule : public IModuleInterface
{
public:

	static inline IBleUtilitiesModule& Get()
	{
		return FModuleManager::LoadModuleChecked< IBleUtilitiesModule >("BleUtilities");
	}

	static inline bool IsAvailable()
	{
		return FModuleManager::Get().IsModuleLoaded("BleUtilities");
	}
};
