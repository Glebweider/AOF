// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AOF/Core/Inventory/Structures/ItemStructure.h"
#include "Components/WidgetComponent.h"
#include "UObject/Interface.h"
#include "ToPlayerInterface.generated.h"

UINTERFACE(MinimalAPI)
class UToPlayerInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class AOF_API IToPlayerInterface
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category="Interfaces")
	void SetNickname(const FString& Nickname);

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category="Interfaces")
	void SetVisibilityButtonInteract(UWidgetComponent* WidgetComponent, const bool bVisibility);

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category="Interfaces")
	void PickUpItem(AActor* ItemPickUp, FInventoryItem InventoryItemPickUp);

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category="Interfaces")
	void TakeMagazine();
};
