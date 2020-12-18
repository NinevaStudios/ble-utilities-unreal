//Copyright (c) 2020 Nineva Studios

#pragma once

#include "Kismet/BlueprintFunctionLibrary.h"

#include "Interfaces/IPluginManager.h"
#include "PluginDescriptor.h"

#include "BleUtilitiesPluginUtils.generated.h"

UCLASS()
class UBleUtilitiesPluginUtils : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:

	/**
	* Get plugin version name
	*
	* @param PluginName - name of the plugin to get info from
	*/
	UFUNCTION(BlueprintCallable, Category = "Ble Utilities | Plugin Utils")
		static FString GetPluginVersionName(FString PluginName);
};

