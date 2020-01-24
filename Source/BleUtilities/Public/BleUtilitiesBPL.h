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
	static TScriptInterface<IBLEManagerInterface> CreateBLEManager(TArray<FString> serviceUUIDs);

	/**
	* Create new instance of BLE scanner
	*
	* @return - reference to BLE scanner interface
	*/
	UFUNCTION(BlueprintCallable, Category = "BleUtilities")
	static TScriptInterface<IBLEScannerInterface> CreateBLEScanner();


	/**
	 * Convert array of bytes to HEX string
	 *
	 * @return - hex string representing array of bytes
	 */
	UFUNCTION(BlueprintPure, Category = "BleUtilities")
	static FString ConvertBytesToHexString(TArray<uint8> bytes);

	/**
	* Parse a 16 bit hex value into an UUID string
	*
	* @return - FString
	*/
	UFUNCTION(BlueprintPure, Category = "BleUtilities")
	static FString HexToUUID(FString hexString);

	/**
	* Parse an array of 16 bit hex values into an array of UUID strings
	*
	* @return - array of FString
	*/
	UFUNCTION(BlueprintPure, Category = "BleUtilities")
	static TArray<FString> HexToUUIDs(TArray<FString> hexStrings);
};
