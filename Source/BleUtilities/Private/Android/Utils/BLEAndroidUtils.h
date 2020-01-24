// Copyright (c) 2019 Nineva Studios

#pragma once

#include "Android/AndroidApplication.h"
#include "Android/AndroidJava.h"

class BLEAndroidUtils
{
	public:

	static jobject GetJavaString(FString string);
	static FString GetFString(jstring string);
	static TArray<uint8> ConvertToByteArray(jbyteArray javaArray);
	static jbyteArray GetJavaArray(TArray<uint8> data);
	static jobjectArray GetJavaArray(TArray<FString> data);
};
