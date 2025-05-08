// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "ItemStructure.generated.h"

/**
 * 
 */
USTRUCT(BlueprintType)
struct AOF_API FInventoryItem
{
	GENERATED_BODY()

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Item")
	FName ItemName;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Item")
	int32 Quantity;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Item")
	float MaxDurability;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Item")
	float Durability;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Item")
	bool bCanBeUsed;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Item")
	TSubclassOf<AActor> ItemClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Item")
	UTexture2D* Icon;
};