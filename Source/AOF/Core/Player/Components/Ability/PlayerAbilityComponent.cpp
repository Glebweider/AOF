// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerAbilityComponent.h"

#include "AOF/Core/Player/Interfaces/ToPlayer/ToPlayerInterface.h"
#include "AOF/Core/Weapon/Interface/ToWeapon/ToWeaponInterface.h"
#include "Blueprint/UserWidget.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Net/UnrealNetwork.h"


void UPlayerAbilityComponent::BeginPlay()
{
	Super::BeginPlay();

	Health = MaxHealth;
	Stamina = MaxStamina;
	PlayerCharacter = Cast<ACharacter>(GetOwner());
	PlayerController = Cast<APlayerController>(PlayerCharacter->GetController());
	CharacterMovement = PlayerCharacter->GetComponentByClass<UCharacterMovementComponent>();
}


void UPlayerAbilityComponent::TakeDamage(float Damage, AActor* Character)
{
	Health = Health - Damage;
	if (Health <= 0)
	{
		Client_PlayerDie();
	} else
	{
		//Feeling and Bleeding
	}
	
	//if (!Cast<ACharacter>(Character))
	//{
		
	//}
}

void UPlayerAbilityComponent::Client_PlayerDie_Implementation()
{
	if (PlayerController && PlayerController->IsPlayerController())
	{
		Server_PlayerDie();
		
		if (DeadScreenWidgetClass)
		{
			UUserWidget* DeadScreenWidget = CreateWidget<UUserWidget>(GetWorld(), DeadScreenWidgetClass);
			if (DeadScreenWidget)
			{
				DeadScreenWidget->AddToViewport();
			}
		}
	}
}

void UPlayerAbilityComponent::Server_PlayerDie_Implementation()
{
	Multi_PlayerDie();
}

void UPlayerAbilityComponent::Multi_PlayerDie_Implementation()
{
	UCapsuleComponent* Capsule = PlayerCharacter->GetComponentByClass<UCapsuleComponent>();
	if (Capsule)
	{
		Capsule->SetCollisionEnabled(ECollisionEnabled::PhysicsOnly);
	}

	USkeletalMeshComponent* SkeletalMeshComponent = PlayerCharacter->GetComponentByClass<USkeletalMeshComponent>();
	if (SkeletalMeshComponent)
	{
		SkeletalMeshComponent->SetCollisionObjectType(ECollisionChannel::ECC_PhysicsBody);
		SkeletalMeshComponent->SetCollisionEnabled(ECollisionEnabled::PhysicsOnly);
		SkeletalMeshComponent->SetSimulatePhysics(true);
	}
	
	if (CharacterMovement)
	{
		CharacterMovement->DisableMovement();
	}

	// Remove UI(but Dead Screen) FUNC ->
}

void UPlayerAbilityComponent::Client_Sprint_Implementation(bool bIsNewSprint)
{
	if (!PlayerCharacter) return;

	auto ItemInHand = IToPlayerInterface::Execute_GetItemInHand(PlayerCharacter);
	if (ItemInHand && ItemInHand->Implements<UToWeaponInterface>())
	{
		bool bIsReloading = IToWeaponInterface::Execute_GetIsReloading(ItemInHand);
		if (bIsReloading && bIsRegenStamina) return;
	}
	if (CharacterMovement->Velocity.Size2D() < 10.0f)
	{
		bIsNewSprint = false;
	}
	
	//PlayerCharacter->GetCharacterMovement()->MaxWalkSpeed = bIsNewSprint ? 600.0f : 300.0f;
	//bIsSprint = bIsNewSprint;
	
	Server_Sprint(bIsNewSprint);
}

void UPlayerAbilityComponent::Server_Sprint_Implementation(bool bIsNewSprint)
{
	Multi_Sprint(bIsNewSprint);
}

void UPlayerAbilityComponent::Multi_Sprint_Implementation(bool bIsNewSprint)
{
	if (bIsNewSprint)
	{
		if (Stamina != 0 && Stamina >= 15)
		{
			UKismetSystemLibrary::K2_ClearAndInvalidateTimerHandle(this, StaminaRegenTimerHandle);
			CharacterMovement->MaxWalkSpeed = 600.0f;

			GetWorld()->GetTimerManager().SetTimer(
				StaminaSpendTimerHandle,
				this,
				&UPlayerAbilityComponent::UpdateStamina,
				0.1f,
				true);
		}
	} else
	{
		UKismetSystemLibrary::K2_ClearAndInvalidateTimerHandle(this, StaminaSpendTimerHandle);
		CharacterMovement->MaxWalkSpeed = 300.0f;

		if (!StaminaSpendTimerHandle.IsValid())
		{
			GetWorld()->GetTimerManager().SetTimer(
				StaminaRegenTimerHandle,
				this,
				&UPlayerAbilityComponent::UpdateStaminaRegen,
				0.1f,
				true);
		}
	}
}

void UPlayerAbilityComponent::UpdateStamina()
{
	if (CharacterMovement->Velocity.Size2D() != 0.0f && Stamina >= 15.0f)
	{
		Stamina = UKismetMathLibrary::Clamp(Stamina - 0.9f, 0.0f, MaxStamina);
		if (Stamina == 0.0f)
		{
			UKismetSystemLibrary::K2_ClearAndInvalidateTimerHandle(this, StaminaSpendTimerHandle);
			UpdateStaminaRegen();
		}
	} else
	{
		Client_Sprint(false);
	}
}

void UPlayerAbilityComponent::UpdateStaminaRegen()
{
	if (Stamina != MaxStamina)
	{
		Stamina = FMath::Clamp(Stamina + 0.8f, 0.0f, MaxStamina);
		if (Stamina == MaxStamina)
		{
			bIsRegenStamina = false;
			UKismetSystemLibrary::K2_ClearAndInvalidateTimerHandle(this, StaminaRegenTimerHandle);
		} else
		{
			bIsRegenStamina = Stamina >= 50.0f ? true : false;
		}
	}
}

void UPlayerAbilityComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(UPlayerAbilityComponent, Health);
	DOREPLIFETIME(UPlayerAbilityComponent, Stamina);
}