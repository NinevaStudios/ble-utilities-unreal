// Copyright (c) 2019 Nineva Studios

#include "TestBLEUtils.h"

UTestBLEUtilsBPL::UTestBLEUtilsBPL(const FObjectInitializer& ObjectInitializer)
    : Super(ObjectInitializer)
{
}

FString UTestBLEUtilsBPL::ConvertBytesToHexString(TArray<uint8> bytes)
{
    FString str = BytesToHex(bytes.GetData(), bytes.Num());
    
    return str;
}
