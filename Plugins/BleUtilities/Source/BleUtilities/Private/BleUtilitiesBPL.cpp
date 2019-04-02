// Copyright (c) 2019 Nineva Studios

#include "BleUtilitiesBPL.h"

#if PLATFORM_IOS || PLATFORM_ANDROID
#include "BLEManager.h"
#endif

UBleUtilitiesUtilsBPL::UBleUtilitiesUtilsBPL(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

TScriptInterface<IBLEManagerInterface> UBleUtilitiesUtilsBPL::CreateBLEManager()
{
	UE_LOG(LogTemp, Warning, TEXT("BleUtilities => UBleUtilitiesUtilsBPL::CreateBLEManager()"));

#if PLATFORM_IOS || PLATFORM_ANDROID
	UBLEManager* BLEManager = NewObject<UBLEManager>();
	TScriptInterface<IBLEManagerInterface> BLEManagerInterface;
	BLEManagerInterface.SetObject(BLEManager);
	BLEManagerInterface.SetInterface(Cast<IBLEManagerInterface>(BLEManager));
	return BLEManagerInterface;
#endif

	return nullptr;
}