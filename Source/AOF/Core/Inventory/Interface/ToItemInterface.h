// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "ToItemInterface.generated.h"

UINTERFACE()
class UToItemInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class AOF_API IToItemInterface
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category="Interfaces")
	void InteractItem(AActor* CharacterInteract);
};