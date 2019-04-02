// Copyright (c) 2019 Nineva Studios

#include "IBleUtilitiesModule.h"

#define LOCTEXT_NAMESPACE "BleUtilities"

class FBleUtilitiesModule : public IBleUtilitiesModule
{
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;
};

IMPLEMENT_MODULE(FBleUtilitiesModule, BleUtilities)

void FBleUtilitiesModule::StartupModule()
{
	// This code will execute after your module is loaded into memory; the exact timing is specified in the .uplugin file per-module
}

void FBleUtilitiesModule::ShutdownModule()
{
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.
}

#undef LOCTEXT_NAMESPACE
