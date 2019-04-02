// Copyright (c) 2019 Nineva Studios

#pragma once

#include "Interface/BLEManagerInterface.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "BleUtilitiesBPL.generated.h"

UCLASS()
class BLEUTILITIES_API UBleUtilitiesUtilsBPL : public UBlueprintFunctionLibrary
{
	GENERATED_UCLASS_BODY()

	/**
	* Create new instance of BLE manager
	*
	* @return - reference to BLE manager interface
	*/
	UFUNCTION(BlueprintCallable, Category = "BleUtilities")
	static TScriptInterface<IBLEManagerInterface> CreateBLEManager();
};
