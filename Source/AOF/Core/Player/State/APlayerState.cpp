// Fill out your copyright notice in the Description page of Project Settings.


#include "APlayerState.h"

#include "AOF/Core/Player/Interfaces/ToPlayer/ToPlayerInterface.h"
#include "GameFramework/PlayerController.h"


void AAPlayerState::BeginPlay()
{
	Super::BeginPlay();

	APlayerController* PlayerController = Cast<APlayerController>(GetOwner());
	if (PlayerController)
	{
		Server_SendMyNicknameToClient(GetPlayerName());
	}
}

void AAPlayerState::Server_SendMyNicknameToClient_Implementation(const FString& Nickname)
{
	if (Nickname.IsEmpty()) return;

	APawn* PlayerPawn = GetPawn();
	if (PlayerPawn && PlayerPawn->Implements<UToPlayerInterface>())
	{
		IToPlayerInterface::Execute_SetNickname(PlayerPawn, Nickname);
	}
}
