// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerAbilityComponent.h"

#include "Blueprint/UserWidget.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"


UPlayerAbilityComponent::UPlayerAbilityComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UPlayerAbilityComponent::BeginPlay()
{
	Super::BeginPlay();

	Health = MaxHealth;
	Player = Cast<ACharacter>(GetOwner());
	PlayerController = Cast<APlayerController>(Player->GetController());
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
			UUserWidget* MyWidget = CreateWidget<UUserWidget>(GetWorld(), DeadScreenWidgetClass);
			if (MyWidget)
			{
				MyWidget->AddToViewport();
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
	UCapsuleComponent* Capsule = Player->GetComponentByClass<UCapsuleComponent>();
	if (Capsule)
	{
		Capsule->SetCollisionEnabled(ECollisionEnabled::PhysicsOnly);
	}

	USkeletalMeshComponent* SkeletalMeshComponent = Player->GetComponentByClass<USkeletalMeshComponent>();
	if (SkeletalMeshComponent)
	{
		SkeletalMeshComponent->SetCollisionObjectType(ECollisionChannel::ECC_PhysicsBody);
		SkeletalMeshComponent->SetCollisionEnabled(ECollisionEnabled::PhysicsOnly);
		SkeletalMeshComponent->SetSimulatePhysics(true);
	}

	UCharacterMovementComponent* MoveComp = Player->GetComponentByClass<UCharacterMovementComponent>();
	if (MoveComp)
	{
		MoveComp->DisableMovement();
	}

	// Remove UI(but Dead Screen) FUNC ->
}