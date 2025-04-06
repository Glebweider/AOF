// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Structures/ItemStructure.h"
#include "ItemBase.generated.h"

UCLASS()
class AOF_API AItemBase : public AActor
{
	GENERATED_BODY()

public:
	AItemBase();

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item")
	FInventoryItem ItemData;
};