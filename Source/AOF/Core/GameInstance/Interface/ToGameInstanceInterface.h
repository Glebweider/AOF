// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "ToGameInstanceInterface.generated.h"

// This class does not need to be modified.
UINTERFACE()
class UToGameInstanceInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class AOF_API IToGameInstanceInterface
{
	GENERATED_BODY()

public:
	
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category="Server")
	void CreateServer();

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category="Server")
	void JoinServer();
};
