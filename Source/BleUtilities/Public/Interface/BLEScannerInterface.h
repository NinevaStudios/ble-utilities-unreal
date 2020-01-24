// Copyright (c) 2019 Nineva Studios

#pragma once

#include "UObject/Interface.h"
#include "BLEScannerInterface.generated.h"

DECLARE_DYNAMIC_DELEGATE_TwoParams(FOnAdvertisementReceivedDelegate, FString, Name, const TArray<uint8>&, data);

UINTERFACE(BlueprintType, meta = (CannotImplementInterfaceInBlueprint))
class BLEUTILITIES_API UBLEScannerInterface : public UInterface
{
	GENERATED_BODY()
};

class BLEUTILITIES_API IBLEScannerInterface
{
	GENERATED_BODY()

public:

	/**
	* Start Scan
	*/
	UFUNCTION(BlueprintCallable, Category = "BleUtilities", meta = (AutoCreateRefTerm = "nameFilters"))
	virtual void StartScan(const FOnAdvertisementReceivedDelegate& onAdvertisementReceivedDelegate,
		const TArray<FString>& nameFilters) = 0;

	/**
	* Stop Scan
	*/
	UFUNCTION(BlueprintCallable, Category = "BleUtilities")
	virtual void StopScan() = 0;
};
