// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AOF/Core/Compass/Structures/CompassMarkerStructure.h"
#include "UObject/Interface.h"
#include "ToUIInterface.generated.h"

UINTERFACE()
class UToUIInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class AOF_API IToUIInterface
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category="Interfaces")
	void SetPlayerNickname(const FString& Nickname);
	
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category="Interfaces")
	void SetVisibilityInventory(bool bIsInventoryOpen);

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category="Interfaces")
	void AddMarkerToPlayerCompass(ECompassMarkerType MarkerType, AActor* Actor);
};
