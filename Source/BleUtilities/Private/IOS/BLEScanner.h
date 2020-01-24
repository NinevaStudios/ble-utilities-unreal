// Copyright (c) 2020 Nineva Studios

#pragma once

#include "Interface/BLEScannerInterface.h"

#if PLATFORM_IOS
#import <CoreBluetooth/CoreBluetooth.h>
@class BLEScannerDelegate;
#endif

#include "BLEScanner.generated.h"

UCLASS()
class UBLEScanner : public UObject, public IBLEScannerInterface
{
	GENERATED_UCLASS_BODY()

public:

	virtual ~UBLEScanner() = default;

	virtual void PostInitProperties() override;
	virtual void BeginDestroy() override;

	virtual void StartScan(const FOnAdvertisementReceivedDelegate& onAdvertisementReceivedDelegate,
		const TArray<FString>& nameFilters) override;
	virtual void StopScan() override;

private:
	CBCentralManager* CentralManager;
	BLEScannerDelegate* ScannerDelegate;

	FOnAdvertisementReceivedDelegate OnAdvertisementReceivedDelegate;
};
