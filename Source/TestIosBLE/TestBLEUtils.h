// Copyright (c) 2019 Nineva Studios

#pragma once

#include "Kismet/BlueprintFunctionLibrary.h"
#include "TestBLEUtils.generated.h"

UCLASS()
class TESTIOSBLE_API UTestBLEUtilsBPL : public UBlueprintFunctionLibrary
{
    GENERATED_UCLASS_BODY()
    
    /**
     * Convert array of bytes to HEX string
     *
     * @return - HEX string representing array of bytes
     */
    UFUNCTION(BlueprintCallable, Category = "IosBLETest")
    static FString ConvertBytesToHexString(TArray<uint8> bytes);
};
