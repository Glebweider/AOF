// Fill out your copyright notice in the Description page of Project Settings.


#include "InventoryComponent.h"
#include "Net/UnrealNetwork.h"


UInventoryComponent::UInventoryComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
	SetIsReplicatedByDefault(true);
	MaxSlots = 24;
}

void UInventoryComponent::Server_SpawnItemInHand_Implementation(int32 ItemID)
{
	ItemID = ItemID - 1;
	if (!InventoryItems.IsEmpty() && InventoryItems.IsValidIndex(ItemID))
	{
		FInventoryItem Item = InventoryItems[ItemID];
		
		if (SelectedItemInHand)
		{
			if (SelectedItemInHand->GetClass() == Item.ItemClass)
			{
				return;
			}
			
			SelectedItemInHand->Destroy();
			SelectedItemInHand = nullptr;
		}
		
		if (Item.bCanBeUsed)
		{
			if (Item.ItemClass)
			{
				FActorSpawnParameters SpawnParams;
				SpawnParams.Owner = GetOwner();
				SpawnParams.Instigator = GetOwner()->GetInstigator();
				SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

				SelectedItemInHand = GetWorld()->SpawnActor<AActor>(Item.ItemClass, SpawnParams);
				if (SelectedItemInHand)
				{
					SelectedItemInHand->AttachToComponent(
						GetOwner()->FindComponentByClass<USkeletalMeshComponent>(),
						FAttachmentTransformRules::SnapToTargetNotIncludingScale,
						FName("skt_rifle")
					);
				}
			}			
		}
	}
}

bool UInventoryComponent::AddItem(FInventoryItem Item)
{
	if (InventoryItems.Num() > MaxSlots) return false;
	
	//Item.Quantity
	InventoryItems.Add(Item);
	if (Cast<APlayerController>(GetOwner()->GetInstigatorController())->IsLocalController())
	{
		OnInventoryChanged.Broadcast();
	}
	return true;
}

bool UInventoryComponent::RemoveItem(FName ItemID, int32 Quantity)
{
	if (Cast<APlayerController>(GetOwner()->GetInstigatorController())->IsLocalController())
	{
		OnInventoryChanged.Broadcast();
	}
	return false;
}

void UInventoryComponent::OnRep_InventoryItems()
{
	OnInventoryChanged.Broadcast();
}

void UInventoryComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(UInventoryComponent, SelectedItemInHand);
	DOREPLIFETIME(UInventoryComponent, InventoryItems);
}