// Copyright (c) 2018 Nineva Studios

#include "BLEManager.h"
#include "BLEDevice.h"
#include "BLEManagerHolder.h"

UBLEManager::UBLEManager(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
	, Guid(FGuid::NewGuid())
	, BLEManagerJavaObject(nullptr)
{
	UE_LOG(LogTemp, Warning, TEXT("BLEUTILS => UBLEManager::UBLEManager()"));

	if (UBLEManager::StaticClass()->GetDefaultObject() != this)
	{
		// create manager
		BLEManagerJavaObject = MakeShareable(new FJavaClassObject("com/ninevastudios/androidbleunrealapi/BleUtilitiesManager", "(Landroid/app/Activity;Ljava/lang/String;)V",
			FJavaWrapper::GameActivityThis, FJavaClassObject::GetJString(Guid.ToString())));

		// get references to Java methods
		StartScanMethod = BLEManagerJavaObject->GetClassMethod("startScan", "()V");
		StopScanMethod = BLEManagerJavaObject->GetClassMethod("stopScan", "()V");
		IsBluetoothEnabledMethod = BLEManagerJavaObject->GetClassMethod("isBluetoothEnabled", "()Z");
		IsBleSupportedMethod = BLEManagerJavaObject->GetClassMethod("supportsBle", "()Z");
		SetBluetoothEnabled = BLEManagerJavaObject->GetClassMethod("setBluetoothEnabled", "(Z)V");
		ConnectToDeviceMethod = BLEManagerJavaObject->GetClassMethod("connect", "(Ljava/lang/String;)V");
		DisconnectDeviceMethod = BLEManagerJavaObject->GetClassMethod("disconnect", "(Ljava/lang/String;)V");
		IsDeviceConnected = BLEManagerJavaObject->GetClassMethod("isConnected", "(Ljava/lang/String;)Z");
		GetDeviceNameMethod = BLEManagerJavaObject->GetClassMethod("getDeviceName", "(Ljava/lang/String;)Ljava/lang/String;");
		SubscribeForNotificationsMethod = BLEManagerJavaObject->GetClassMethod("subscribe", "(Ljava/lang/String;)V");
		StopNotificationsMethod = BLEManagerJavaObject->GetClassMethod("unsubscribe", "(Ljava/lang/String;)V");
		SetDeviceGuidMethod = BLEManagerJavaObject->GetClassMethod("setDeviceId", "(Ljava/lang/String;Ljava/lang/String;)V");
	}
}

UBLEManager::~UBLEManager()
{
	UE_LOG(LogTemp, Warning, TEXT("BLEUTILS => UBLEManager::~UBLEManager()"));
}

void UBLEManager::PostInitProperties()
{
	UE_LOG(LogTemp, Warning, TEXT("BLEUTILS => UBLEManager::PostInitProperties()"));

	Super::PostInitProperties();

	if (UBLEManager::StaticClass()->GetDefaultObject() != this)
	{
		// add BLE manager to root to avoid GC issues
		AddToRoot();
		
		BLEManagerHolder::GetInstance()->AddManager(Guid, this);
	}
}

void UBLEManager::BeginDestroy()
{
	UE_LOG(LogTemp, Warning, TEXT("BLEUTILS => UBLEManager::BeginDestroy()"));

	if (UBLEManager::StaticClass()->GetDefaultObject() != this)
	{
		BLEManagerHolder::GetInstance()->RemoveManager(Guid);

		// remove BLE manager from root so now it can be safely destroyed by GC
		RemoveFromRoot();
	}

	Super::BeginDestroy();
}

bool UBLEManager::CheckBleSupport()
{
	UE_LOG(LogTemp, Warning, TEXT("BLEUTILS => UBLEManager::CheckBleSupport()"));
	
	return BLEManagerJavaObject->CallMethod<bool>(IsBleSupportedMethod);
}

bool UBLEManager::IsBluetoothEnabled()
{
	UE_LOG(LogTemp, Warning, TEXT("BLEUTILS => UBLEManager::IsBluetoothEnabled()"));

	return BLEManagerJavaObject->CallMethod<bool>(IsBluetoothEnabledMethod);
}

void UBLEManager::SetBluetoothState(bool enable)
{
	UE_LOG(LogTemp, Warning, TEXT("BLEUTILS => UBLEManager::SetBluetoothState()"));
	
	BLEManagerJavaObject->CallMethod<void>(SetBluetoothEnabled, enable);
}

void UBLEManager::ScanForDevices(const FOnDeviceFoundDelegate& onDeviceFoundCallback)
{
	UE_LOG(LogTemp, Warning, TEXT("BLEUTILS => UBLEManager::ScanForDevices()"));

	OnDeviceFoundDelegate = onDeviceFoundCallback;

	BLEManagerJavaObject->CallMethod<void>(StartScanMethod);
}

void UBLEManager::StopScan()
{
	UE_LOG(LogTemp, Warning, TEXT("BLEUTILS => UBLEManager::StopScan()"));

	BLEManagerJavaObject->CallMethod<void>(StopScanMethod);
}

void UBLEManager::ConnectToDevice(TScriptInterface<IBLEDeviceInterface> device,
	const FOnDeviceConnectedDelegate& onDeviceConnectedCallback, const FOnConnectionErrorDelegate& onFailToConnectDeviceCallback)
{
	UE_LOG(LogTemp, Warning, TEXT("BLEUTILS => UBLEManager::ConnectToDevice()"));

	OnDeviceConnectedDelegate = onDeviceConnectedCallback;
	OnConnectionErrorDelegate = onFailToConnectDeviceCallback;

	UBLEDevice* BLEDevice = dynamic_cast<UBLEDevice*>(device.GetObject());

	BLEManagerJavaObject->CallMethod<void>(ConnectToDeviceMethod, FJavaClassObject::GetJString(BLEDevice->GetDeviceInternal()));
}

void UBLEManager::DisconnectDevice(TScriptInterface<IBLEDeviceInterface> device,
	const FOnDeviceDisconnectedDelegate& onDeviceDisconnectedCallback, const FOnDisconnectionErrorDelegate& onFailToDosconnectDeviceCallback)
{
	UE_LOG(LogTemp, Warning, TEXT("BLEUTILS => UBLEManager::DisconnectDevice()"));

	OnDeviceDisconnectedDelegate = onDeviceDisconnectedCallback;
	OnDisconnectionErrorDelegate = onFailToDosconnectDeviceCallback;

	UBLEDevice* BLEDevice = dynamic_cast<UBLEDevice*>(device.GetObject());

	BLEManagerJavaObject->CallMethod<void>(DisconnectDeviceMethod, FJavaClassObject::GetJString(BLEDevice->GetDeviceInternal()));
}

void UBLEManager::OnDeviceFound(FString deviceId)
{
	UBLEDevice* BLEDevice = NewObject<UBLEDevice>();
	BLEDevice->SetDeviceInternal(deviceId);
	BLEDevice->SetManagerInternal(Guid);
		
	BLEManagerJavaObject->CallMethod<void>(SetDeviceGuidMethod, FJavaClassObject::GetJString(deviceId), FJavaClassObject::GetJString(BLEDevice->GetDeviceGuid().ToString()));

	TScriptInterface<IBLEDeviceInterface> BLEDeviceInterface;
	BLEDeviceInterface.SetObject(BLEDevice);
	BLEDeviceInterface.SetInterface(Cast<IBLEDeviceInterface>(BLEDevice));

	OnDeviceFoundDelegate.ExecuteIfBound(BLEDevice);
}

void UBLEManager::OnDeviceConnected()
{
	OnDeviceConnectedDelegate.ExecuteIfBound();
}

void UBLEManager::OnFailedToConnectDevice(FString error)
{
	OnConnectionErrorDelegate.ExecuteIfBound(error);
}

void UBLEManager::OnDeviceDisconnected()
{
	OnDeviceDisconnectedDelegate.ExecuteIfBound();
}

void UBLEManager::OnFailedToDisconnectDevice(FString error)
{
	OnDisconnectionErrorDelegate.ExecuteIfBound(error);
}

FString UBLEManager::GetDeviceName(FString deviceId)
{	
	jstring Return = static_cast<jstring>(BLEManagerJavaObject->CallMethod<jobject>(GetDeviceNameMethod, FJavaClassObject::GetJString(deviceId)));

	JNIEnv* Env = FAndroidApplication::GetJavaEnv();
	const char * UTFString = Env->GetStringUTFChars(Return, nullptr);
	FString Result(UTF8_TO_TCHAR(UTFString));
	Env->ReleaseStringUTFChars(Return, UTFString);

	return Result;
}

bool UBLEManager::CheckIsDeviceConnected(FString deviceId)
{
	return BLEManagerJavaObject->CallMethod<bool>(IsDeviceConnected, FJavaClassObject::GetJString(deviceId));
}

void UBLEManager::SubscribeForNotifications(FString deviceId)
{
	BLEManagerJavaObject->CallMethod<void>(SubscribeForNotificationsMethod, FJavaClassObject::GetJString(deviceId));
}

void UBLEManager::StopNotifications(FString deviceId)
{
	BLEManagerJavaObject->CallMethod<void>(StopNotificationsMethod, FJavaClassObject::GetJString(deviceId));
}

