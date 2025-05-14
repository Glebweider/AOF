// Fill out your copyright notice in the Description page of Project Settings.


#include "APlayerCharacter.h"

#include "AOF/Core/Inventory/Component/Inventory/InventoryComponent.h"
#include "AOF/Core/Inventory/Interface/ToItemInterface.h"
#include "AOF/Core/Player/Components/UI/PlayerUIComponent.h"
#include "AOF/Core/Weapon/Interface/ToWeapon/ToWeaponInterface.h"
#include "AOF/UI/Interface/ToUIInterface.h"
#include "Camera/CameraComponent.h"
#include "Components/WidgetComponent.h"
#include "Net/UnrealNetwork.h"


AAPlayerCharacter::AAPlayerCharacter()
{
	PrimaryActorTick.bCanEverTick = true;
	
	PlayerNameTagComponent = CreateDefaultSubobject<UWidgetComponent>(TEXT("PlayerNameTagWidget"));
	if (PlayerNameTagComponent && RootComponent)
	{
		PlayerNameTagComponent->SetupAttachment(RootComponent);
	}
	
	DetectionSphere = CreateDefaultSubobject<USphereComponent>(TEXT("DetectionSphere"));
	if (DetectionSphere && RootComponent)
	{
		DetectionSphere->SetupAttachment(RootComponent);
		DetectionSphere->SetSphereRadius(800.f);

		DetectionSphere->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
		DetectionSphere->SetCollisionObjectType(ECollisionChannel::ECC_WorldDynamic);
		DetectionSphere->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
		DetectionSphere->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Overlap);

		DetectionSphere->OnComponentBeginOverlap.AddDynamic(this, &AAPlayerCharacter::OnBeginOverlap);
		DetectionSphere->OnComponentEndOverlap.AddDynamic(this, &AAPlayerCharacter::OnEndOverlap);		
	}
}

void AAPlayerCharacter::BeginPlay()
{
	Super::BeginPlay();

	BP_PlayerController = Cast<APlayerController>(GetController());
	
	InventoryComponent = FindComponentByClass<UInventoryComponent>();
	PlayerAbilityComponent = FindComponentByClass<UPlayerAbilityComponent>();
	PlayerUIComponent = FindComponentByClass<UPlayerUIComponent>();
	SkeletalMeshComponent = FindComponentByClass<USkeletalMeshComponent>();

	if (PlayerUIComponent)
	{
		PlayerUIComponent->CreatePlayerWidget(WidgetPlayerHUD);
	}
}

void AAPlayerCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	if (IsLocallyControlled() || HasAuthority())
	{
		FRotator PawnRotation = GetControlRotation();
		float RotatorX = (PawnRotation.Pitch > 180.0f ? 360.0f - PawnRotation.Pitch : PawnRotation.Pitch * -1.0f) / 3.0f;
		
		ControlRotationSync = FRotator(0.0f, 0.0f, RotatorX);
	}
}

void AAPlayerCharacter::OnBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor && OtherActor->Implements<UToPlayerInterface>())
	{
		IToPlayerInterface::Execute_SetVisibilityUIWidget(OtherActor, PlayerNameTagComponent, true);
	}
}

void AAPlayerCharacter::OnEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (OtherActor && OtherActor->Implements<UToPlayerInterface>())
	{
		IToPlayerInterface::Execute_SetVisibilityUIWidget(OtherActor, PlayerNameTagComponent, false);
	}
}

void AAPlayerCharacter::SetNickname_Implementation(const FString& Nickname)
{
	if (!PlayerNameTagComponent) return;

	UUserWidget* Widget = PlayerNameTagComponent->GetWidget();
	if (Widget && Widget->Implements<UToUIInterface>())
	{
		IToUIInterface::Execute_SetPlayerNickname(Widget, Nickname);
	}
}

void AAPlayerCharacter::SetVisibilityUIWidget_Implementation(UWidgetComponent* WidgetComponent, const bool bVisibility)
{
	if (BP_PlayerController && BP_PlayerController->IsPlayerController() && WidgetComponent)
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

void AAPlayerCharacter::HandleInventory_Implementation()
{
	if (!bIsInventoryOpen && !CanCreateUI())
	{
		return;
	}

	bIsInventoryOpen = !bIsInventoryOpen;
	if (PlayerUIComponent && WidgetPlayerHUD)
	{
		UUserWidget* PlayerHUDWidget = PlayerUIComponent->GetPlayerWidget(WidgetPlayerHUD);
		if (PlayerHUDWidget)
		{
			IToUIInterface::Execute_SetVisibilityInventory(PlayerHUDWidget, bIsInventoryOpen);
		}

		SetUIActive(bIsInventoryOpen, bIsInventoryOpen, bIsInventoryOpen);
	}
}

void AAPlayerCharacter::Server_Interact_Implementation(AActor* ItemPickUp, FInventoryItem InventoryItemPickUp)
{
	if (AddItemToInventory(ItemPickUp, InventoryItemPickUp))
	{
		Multi_Interact(ItemPickUp, InventoryItemPickUp);
	}
}

void AAPlayerCharacter::Server_TakeMagazine_Implementation()
{
	Multi_TakeMagazine();
}

void AAPlayerCharacter::Server_Crouch_Implementation(bool bIsNewCrouch)
{
	Multi_Crouch(bIsNewCrouch);
}

void AAPlayerCharacter::Multi_Interact_Implementation(AActor* ItemPickUp, FInventoryItem InventoryItemPickUp)
{
	if (ItemPickUp)
	{
		ItemPickUp->Destroy();
	}
}

void AAPlayerCharacter::Multi_TakeMagazine_Implementation()
{
	FTransform SpawnTransform;
	SpawnTransform = SkeletalMeshComponent->GetSocketTransform(FName("skt_Mag"), RTS_World);
	
	UStaticMeshComponent* MagMeshComponent = NewObject<UStaticMeshComponent>(this, UStaticMeshComponent::StaticClass(), FName("Magazine"));
	if (MagMeshComponent && SpawnTransform.IsValid())
	{
		MagMeshComponent->RegisterComponent();
		MagMeshComponent->SetWorldTransform(SpawnTransform);
		if (InventoryComponent->SelectedItemInHand->Implements<UToWeaponInterface>())
		{
			UStaticMesh* MagazineStaticMesh = IToWeaponInterface::Execute_GetMagazineMesh(InventoryComponent->SelectedItemInHand);
			if (MagazineStaticMesh)
			{
				MagMeshComponent->SetStaticMesh(MagazineStaticMesh);
				IToWeaponInterface::Execute_SetMagazineVariableSkeletalMeshComponent(InventoryComponent->SelectedItemInHand, MagMeshComponent);
				MagMeshComponent->AttachToComponent(SkeletalMeshComponent, FAttachmentTransformRules::SnapToTargetNotIncludingScale, FName("skt_Mag"));
			}
		}
	}
}

void AAPlayerCharacter::Multi_Crouch_Implementation(bool bIsNewCrouch)
{
	if (bIsNewCrouch)
	{
		Crouch();
	} else
	{
		UnCrouch();
	}
}

bool AAPlayerCharacter::AddItemToInventory(AActor* ItemPickUp, FInventoryItem InventoryItemPickUp)
{
	return InventoryComponent && ItemPickUp && InventoryComponent->AddItem(InventoryItemPickUp);
}

void AAPlayerCharacter::SetUIActive(bool bIsActive, bool bIsIgnoreMove, bool bIsIgnoreLook)
{
	if (BP_PlayerController && PlayerUIComponent)
	{
		PlayerUIComponent->bIsUIActive = bIsActive;
		if (bIsActive)
		{
			BP_PlayerController->SetInputMode(FInputModeGameAndUI());
		}
		else
		{
			BP_PlayerController->SetInputMode(FInputModeGameOnly());
		}
		
		BP_PlayerController->bShowMouseCursor = bIsActive;
		BP_PlayerController->SetIgnoreMoveInput(bIsIgnoreMove);
		BP_PlayerController->SetIgnoreLookInput(bIsIgnoreLook);
	}
}

bool AAPlayerCharacter::CanCreateUI()
{
	return PlayerUIComponent && !PlayerUIComponent->bIsUIActive;
}

void AAPlayerCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(AAPlayerCharacter, ControlRotationSync);
}
