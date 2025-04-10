// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "BPF_Functions.generated.h"

/**
 * 
 */
UCLASS()
class AOF_API UBPF_Functions : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintPure, Category = "Weapon")
	static void GetLookDirection(float Length, UObject* WorldContextObject, FVector& StartDirection, FVector& EndDirection);
};
