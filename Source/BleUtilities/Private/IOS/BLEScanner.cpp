// Copyright (c) 2020 Nineva Studios

#include "BLEScanner.h"
#include "BleLog.h"

#import "BLEScannerDelegate.h"

UBLEScanner::UBLEScanner(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	UE_LOG(LogBle, Verbose, TEXT("UBLEScanner::UBLEScanner()"));

	if (UBLEScanner::StaticClass()->GetDefaultObject() != this)
	{
		ScannerDelegate = [[BLEScannerDelegate alloc] init];

		ScannerDelegate.onAdvertisementReceived = ^ (FString deviceName, NSData* data) {
			TArray<uint8> byteData;
			uint8* byteArray = (uint8*)data.bytes;

			if (byteArray)
			{
				// skip the first two prefix bytes (0x0201)
				const int startIndex = byteArray[0] == 0x02 && byteArray[1] == 0x01 ? 2 : 0;
				for (int i = startIndex; i < data.length; i++)
				{
					byteData.Add(byteArray[i]);
				}
				[data release];
			}

			OnAdvertisementReceivedDelegate.ExecuteIfBound(deviceName, byteData);
		};

		CentralManager = [[CBCentralManager alloc] initWithDelegate:ScannerDelegate queue:nil];
	}
}

void UBLEScanner::PostInitProperties()
{
	Super::PostInitProperties();

	if (UBLEScanner::StaticClass()->GetDefaultObject() != this)
	{
		// add BLE scanner to root to avoid GC issues
		AddToRoot();
	}
}

void UBLEScanner::BeginDestroy()
{
	UE_LOG(LogBle, Verbose, TEXT("UBLEScanner::BeginDestroy()"));

	if (UBLEScanner::StaticClass()->GetDefaultObject() != this)
	{
		// remove BLE scanner from root so now it can be safely destroyed by GC
		RemoveFromRoot();
	}

	Super::BeginDestroy();
}

void UBLEScanner::StartScan(const FOnAdvertisementReceivedDelegate& onAdvertisementReceivedDelegate, const TArray<FString>& nameFilters)
{
	UE_LOG(LogBle, Verbose, TEXT("UBLEScanner::StartScan()"));

	OnAdvertisementReceivedDelegate = onAdvertisementReceivedDelegate;

	NSMutableArray* nameFilterArray = [NSMutableArray new];
	for (const FString& nameFilter : nameFilters)
	{
		[nameFilterArray addObject:nameFilter.GetNSString()];
	}
	ScannerDelegate.nameFilters = nameFilterArray;

	if (CentralManager.state == CBManagerStatePoweredOn)
	{
		[CentralManager scanForPeripheralsWithServices:nil
						options:@{CBCentralManagerScanOptionAllowDuplicatesKey:@NO}];
	}
}

void UBLEScanner::StopScan()
{
	UE_LOG(LogBle, Verbose, TEXT("UBLEScanner::StopScan()"));
	[CentralManager stopScan];
}
