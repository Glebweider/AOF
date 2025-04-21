// Fill out your copyright notice in the Description page of Project Settings.


#include "APlayerCharacter.h"

#include "AOF/Core/Inventory/Component/Inventory/InventoryComponent.h"
#include "AOF/Core/Inventory/Interface/ToItemInterface.h"
#include "AOF/UI/Interface/ToUIInterface.h"
#include "Camera/CameraComponent.h"
#include "Components/WidgetComponent.h"


AAPlayerCharacter::AAPlayerCharacter()
{
	PrimaryActorTick.bCanEverTick = false;
}

void AAPlayerCharacter::BeginPlay()
{
	Super::BeginPlay();

	InventoryComponent = FindComponentByClass<UInventoryComponent>();
	PlayerAbilityComponent = FindComponentByClass<UPlayerAbilityComponent>();
}

void AAPlayerCharacter::SetNickname_Implementation(const FString& Nickname)
{
	const UWidgetComponent* WidgetComp = Cast<UWidgetComponent>(GetDefaultSubobjectByName(TEXT("PlayerNameTagWidget")));
	if (!WidgetComp)
	{
		return;
	}

	UWidget* Widget = WidgetComp->GetWidget();
	if (Widget && Widget->Implements<UToUIInterface>())
	{
		IToUIInterface::Execute_SetPlayerNickname(Widget, Nickname);
	}
}

void AAPlayerCharacter::SetVisibilityButtonInteract_Implementation(UWidgetComponent* WidgetComponent, const bool bVisibility)
{
	if (BP_PlayerController && BP_PlayerController->IsPlayerController())
	{
		WidgetComponent->SetVisibility(bVisibility);
	}
}

void AAPlayerCharacter::HandleInteract_Implementation()
{
	const UCameraComponent* CameraComponent = FindComponentByClass<UCameraComponent>();
	if (CameraComponent && BP_PlayerController)
	{
		int32 ViewportX, ViewportY;
		FVector StartDirection, EndDirection;
	
		BP_PlayerController->GetViewportSize(ViewportX, ViewportY);
		BP_PlayerController->DeprojectScreenPositionToWorld(ViewportX / 2, ViewportY / 2, StartDirection, EndDirection);

		const FRotator CameraRotation = CameraComponent->GetComponentRotation();
		const FVector End = StartDirection + (CameraRotation.Vector() * 220.0f);
		
		FHitResult HitResult;
		FCollisionQueryParams CollisionParams;
		CollisionParams.AddIgnoredActor(this);

		const bool bHit = GetWorld()->LineTraceSingleByChannel(HitResult, StartDirection, End, ECC_Visibility, CollisionParams);
		if (bHit && !HitResult.GetActor()->GetOwner())
		{
			if (HitResult.GetActor()->Implements<UToItemInterface>())
			{
				IToItemInterface::Execute_InteractItem(HitResult.GetActor(), this);
			}
		}

#if WITH_EDITOR
		if (bHit)
		{
			DrawDebugLine(GetWorld(), StartDirection, End, FColor::Green, false, 1, 0, 1);
		}
		else
		{
			DrawDebugLine(GetWorld(), StartDirection, End, FColor::Red, false, 1, 0, 1);
		}
#endif

	}
}

//////////// PICK UP

void AAPlayerCharacter::PickUpItem_Implementation(AActor* ItemPickUp, FInventoryItem InventoryItemPickUp)
{
	Server_Interact(ItemPickUp, InventoryItemPickUp);
}

void AAPlayerCharacter::Server_Interact_Implementation(AActor* ItemPickUp, FInventoryItem InventoryItemPickUp)
{
	if (AddItemToInventory(ItemPickUp, InventoryItemPickUp))
	{
		Multicast_Interact(ItemPickUp, InventoryItemPickUp);
	}
}

void AAPlayerCharacter::Multicast_Interact_Implementation(AActor* ItemPickUp, FInventoryItem InventoryItemPickUp)
{
	if (ItemPickUp)
	{
		ItemPickUp->Destroy();
	}
}

bool AAPlayerCharacter::AddItemToInventory(AActor* ItemPickUp, FInventoryItem InventoryItemPickUp)
{
	return InventoryComponent && ItemPickUp && InventoryComponent->AddItem(InventoryItemPickUp);
}

////////////

void AAPlayerCharacter::TakeDamage_Implementation(float Damage, AActor* Character)
{
	if (PlayerAbilityComponent)
	{
		PlayerAbilityComponent->TakeDamage(Damage, Character);
	}
}