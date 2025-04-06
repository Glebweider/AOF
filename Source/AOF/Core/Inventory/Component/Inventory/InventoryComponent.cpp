// Fill out your copyright notice in the Description page of Project Settings.


#include "InventoryComponent.h"


UInventoryComponent::UInventoryComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}


void UInventoryComponent::BeginPlay()
{
	Super::BeginPlay();
	
}

void UInventoryComponent::AddItem(TSubclassOf<AActor> ItemClass, int32 Quantity)
{
}

bool UInventoryComponent::RemoveItem(FName ItemID, int32 Quantity)
{
	return false;
}
