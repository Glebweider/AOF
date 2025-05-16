// Fill out your copyright notice in the Description page of Project Settings.


#include "CompassMarkerComponent.h"

#include "AOF/Core/Player/Character/APlayerCharacter.h"
#include "AOF/UI/Interface/ToUIInterface.h"
#include "Kismet/GameplayStatics.h"


UCompassMarkerComponent::UCompassMarkerComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
	SetIsReplicated(true);
}

void UCompassMarkerComponent::BeginPlay()
{
	Super::BeginPlay();
	
	FTimerHandle InitCompassTimerHandle;

	GetWorld()->GetTimerManager().SetTimer(
		InitCompassTimerHandle,
		this,
		&UCompassMarkerComponent::InitCompassForLocalPlayer,
		4.0f,
		false
	);
}

void UCompassMarkerComponent::InitCompassForLocalPlayer()
{
	TArray<AActor*> FoundPlayers;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AAPlayerCharacter::StaticClass(), FoundPlayers);
	
	for (AActor* Actor : FoundPlayers)
	{
		APawn* OwnerPawn = Cast<APawn>(Actor);
		if (!OwnerPawn || !OwnerPawn->IsLocallyControlled()) continue;

		auto PlayerUIComponent = Actor->FindComponentByClass<UPlayerUIComponent>();
		if (PlayerUIComponent && GetOwner())
		{
			auto PlayerHUD = PlayerUIComponent->GetPlayerWidget(PlayerHUDWidget);
			if (PlayerHUD && PlayerHUD->Implements<UToUIInterface>())
			{
				IToUIInterface::Execute_AddMarkerToPlayerCompass(PlayerHUD, MarkerType, GetOwner());
			}
		}
	}
}
