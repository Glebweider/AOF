// Fill out your copyright notice in the Description page of Project Settings.


#include "InventoryComponent.h"


UInventoryComponent::UInventoryComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
	MaxSlots = 28;
}


void UInventoryComponent::BeginPlay()
{
	Super::BeginPlay();
	
}

bool UInventoryComponent::AddItem(const FInventoryItem Item)
{
	if (InventoryItems.Num() <= MaxSlots)
	{
		InventoryItems.Add(Item);
		return true;		
	} else
	{
		return false;
	}
}

bool UInventoryComponent::RemoveItem(FName ItemID, int32 Quantity)
{
	return false;
}