// Fill out your copyright notice in the Description page of Project Settings.


#include "InventoryComponent.h"
#include "Net/UnrealNetwork.h"


UInventoryComponent::UInventoryComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
	MaxSlots = 28;
}

void UInventoryComponent::Server_SpawnItemInHand_Implementation(const int32 ItemID)
{
	//const FInventoryItem* Item = FindItemByID(InventoryItems, ItemID)
	if (!InventoryItems.IsEmpty())
	{
		if (InventoryItems.IsValidIndex(ItemID))
		{
			FInventoryItem Item = InventoryItems[ItemID];
			
			UE_LOG(LogTemp, Warning, TEXT("32432432422 %s"), *Item.ItemID.ToString());
			if (SelectedItemInHand)
			{
				SelectedItemInHand->Destroy();
			}

			FActorSpawnParameters SpawnParams;
			SpawnParams.Owner = GetOwner();
			SpawnParams.Instigator = GetOwner()->GetInstigator();
			SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

			if (Item.ItemClass)
			{
				UE_LOG(LogTemp, Warning, TEXT("Spawn ItemClass"));
				SelectedItemInHand = GetWorld()->SpawnActor<AActor>(Item.ItemClass, SpawnParams);
				if (SelectedItemInHand)
				{
					UE_LOG(LogTemp, Warning, TEXT("SelectedItemInHand"));
					SelectedItemInHand->AttachToComponent(GetOwner()->FindComponentByClass<USkeletalMeshComponent>(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, FName("skt_rifle"));
				}
			}
		}
	}
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

void UInventoryComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(UInventoryComponent, SelectedItemInHand);
	DOREPLIFETIME(UInventoryComponent, InventoryItems);
}