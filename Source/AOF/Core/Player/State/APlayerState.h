// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "APlayerState.generated.h"

/**
 * 
 */
UCLASS()
class AOF_API AAPlayerState : public APlayerState
{
	GENERATED_BODY()
	
public:
	virtual void BeginPlay() override;

	UFUNCTION(Server, Reliable)
	void Server_SendMyNicknameToClient(const FString& Nickname);
};
