// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "InputPlayerInterface.generated.h"


UINTERFACE()
class UInputPlayerInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class AOF_API IInputPlayerInterface
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category="Interfaces")
	void HandleInteract();
	
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category="Interfaces")
	void HandleSprint(bool bIsSprint);
};