// Copyright (c) 2019 Nineva Studios

#pragma once

#include "Interface/BLEScannerInterface.h"

#include "Android/AndroidJNI.h"
#include "Android/AndroidApplication.h"
#include "Android/AndroidJava.h"

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

	void OnAdvertisementReceived(FString deviceName, const TArray<uint8>& data);

private:
	TSharedPtr<FJavaClassObject> javaScanner;

	FJavaClassMethod StartScanMethod;
	FJavaClassMethod StopScanMethod;

	FOnAdvertisementReceivedDelegate OnAdvertisementReceivedDelegate;
};
