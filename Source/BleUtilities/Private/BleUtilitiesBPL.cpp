// Copyright (c) 2019 Nineva Studios

#include "BleUtilitiesBPL.h"

#include "BleLog.h"
#include "Misc/Parse.h"

#if PLATFORM_IOS || PLATFORM_ANDROID
#include "BLEManager.h"
#include "BLEScanner.h"
#endif

UBleUtilitiesUtilsBPL::UBleUtilitiesUtilsBPL(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

TScriptInterface<IBLEManagerInterface> UBleUtilitiesUtilsBPL::CreateBLEManager(TArray<FString> serviceUUIDs)
{
#if PLATFORM_IOS || PLATFORM_ANDROID
	UBLEManager* BLEManager = NewObject<UBLEManager>();
	BLEManager->SetServiceUUIDsToScanFor(serviceUUIDs);

	TScriptInterface<IBLEManagerInterface> BLEManagerInterface;
	BLEManagerInterface.SetObject(BLEManager);
	BLEManagerInterface.SetInterface(Cast<IBLEManagerInterface>(BLEManager));
	return BLEManagerInterface;
#endif

	return nullptr;
}

TScriptInterface<IBLEScannerInterface> UBleUtilitiesUtilsBPL::CreateBLEScanner()
{
#if PLATFORM_IOS || PLATFORM_ANDROID
	UBLEScanner* BLEScanner = NewObject<UBLEScanner>();

	TScriptInterface<IBLEScannerInterface> BLEScannerInterface;
	BLEScannerInterface.SetObject(BLEScanner);
	BLEScannerInterface.SetInterface(Cast<IBLEScannerInterface>(BLEScanner));
	return BLEScannerInterface;
#endif

	return nullptr;
}

FString UBleUtilitiesUtilsBPL::ConvertBytesToHexString(TArray<uint8> bytes)
{
	const FString str = BytesToHex(bytes.GetData(), bytes.Num());
	return str;
}

FString UBleUtilitiesUtilsBPL::HexToUUID(FString hexString)
{
	hexString.TrimStartAndEndInline();
	const int startPos = hexString.StartsWith(TEXT("0x")) ? 2 : 0;

	if (hexString.Len() - startPos > 4)
	{
		UE_LOG(LogBle, Error, TEXT("BleUtilities => cannot convert %s as it is not a 16 bit value"), *hexString);
		return FString();
	}

	for (int i = startPos; i < hexString.Len(); i++)
	{
		if (!CheckTCharIsHex(hexString[i]))
		{
			UE_LOG(LogBle, Error, TEXT("BleUtilities => failed to convert %s to a valid UUID"), *hexString);
			return FString();
		}
	}

	uint32 id = FParse::HexNumber(*hexString);
	FGuid guid = FGuid(id, 0x00001000, 0x80000080, 0x5f9b34fb);

	if (!guid.IsValid())
	{
		UE_LOG(LogBle, Error, TEXT("BleUtilities => failed to convert %s to a valid UUID"), *hexString);
		return FString();
	}

	return guid.ToString(EGuidFormats::DigitsWithHyphens);
}

TArray<FString> UBleUtilitiesUtilsBPL::HexToUUIDs(TArray<FString> hexStrings)
{
	TArray<FString> uuidArray;

	for (FString hexString : hexStrings)
	{
		uuidArray.Add(UBleUtilitiesUtilsBPL::HexToUUID(hexString));
	}

	return uuidArray;
}
