﻿// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AOF/Core/Inventory/Structures/ItemStructure.h"
#include "Components/ActorComponent.h"
#include "InventoryComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnInventoryChanged);

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class AOF_API UInventoryComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UInventoryComponent();

	TArray<FInventoryItem>& GetItems() { return InventoryItems; }

	UPROPERTY(BlueprintAssignable, Replicated)
	FOnInventoryChanged OnInventoryChanged;
	
	UFUNCTION(Server, Reliable, BlueprintCallable)
	void Server_SpawnItemInHand(const int32 ItemID);
	
	UFUNCTION(BlueprintCallable, Category = "Inventory")
	bool AddItem(FInventoryItem Item);
	
	UFUNCTION(BlueprintCallable, Category = "Inventory")
	bool RemoveItem(FName ItemID, int32 Quantity = 1);
	
	UPROPERTY(Replicated, VisibleDefaultsOnly, BlueprintReadOnly, Category = "Inventory")
	AActor* SelectedItemInHand;
	
protected:
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	
	UFUNCTION()
	void OnRep_InventoryItems();
	
	UPROPERTY(ReplicatedUsing = OnRep_InventoryItems)
	TArray<FInventoryItem> InventoryItems;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Inventory")
	int32 MaxSlots;
};