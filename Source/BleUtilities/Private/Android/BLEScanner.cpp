// Copyright (c) 2019 Nineva Studios

#include "BLEScanner.h"

#include "BleLog.h"
#include "Utils/BLEAndroidUtils.h"

UBLEScanner::UBLEScanner(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer) 
{
	UE_LOG(LogBle, Verbose, TEXT("UBLEScanner::UBLEScanner()"));

	javaScanner = MakeShareable(new FJavaClassObject("com/ninevastudios/androidbleunrealapi/BleUtilitiesScanner",
			"(Landroid/app/Activity;J)V", FJavaWrapper::GameActivityThis, (jlong)this));

	StartScanMethod = javaScanner->GetClassMethod("startScan", "([Ljava/lang/String;)V");
	StopScanMethod = javaScanner->GetClassMethod("stopScan", "()V");
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

	javaScanner->CallMethod<void>(StartScanMethod, BLEAndroidUtils::GetJavaArray(nameFilters));
}

void UBLEScanner::StopScan()
{
	UE_LOG(LogBle, Verbose, TEXT("UBLEScanner::StopScan()"));

	javaScanner->CallMethod<void>(StopScanMethod);
}

void UBLEScanner::OnAdvertisementReceived(FString deviceName, const TArray<uint8>& data)
{
	UE_LOG(LogBle, Verbose, TEXT("UBLEScanner::OnAdvertisementReceived()"));

	const FString dataString = BytesToHex(data.GetData(), data.Num());
	UE_LOG(LogBle, Verbose, TEXT("Device Name: %s\n\tData: %s"), *deviceName, *dataString);
	
	OnAdvertisementReceivedDelegate.ExecuteIfBound(deviceName, data);
}

#if PLATFORM_ANDROID

JNI_METHOD void Java_com_ninevastudios_androidbleunrealapi_BleUtilitiesScanner_onAdvertisementReceived(JNIEnv* env, jclass clazz, 
	jlong objAddr, jstring name, jbyteArray data)
{
	UE_LOG(LogBle, Verbose, TEXT("Java callback onAdvertisementReceived"));

	FString deviceName = BLEAndroidUtils::GetFString(name);
	TArray<uint8> bytes = BLEAndroidUtils::ConvertToByteArray(data);

	UBLEScanner* scanner = reinterpret_cast<UBLEScanner*>(objAddr);

	AsyncTask(ENamedThreads::GameThread, [=]() {
		scanner->OnAdvertisementReceived(deviceName, bytes);
	});
}

#endif
