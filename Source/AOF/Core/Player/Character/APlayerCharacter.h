// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AOF/Core/Player/Interfaces/InputPlayer/InputPlayerInterface.h"
#include "AOF/Core/Player/Interfaces/ToPlayer/ToPlayerInterface.h"
#include "GameFramework/Character.h"
#include "GameFramework/PlayerController.h"
#include "APlayerCharacter.generated.h"

UCLASS()
class AOF_API AAPlayerCharacter : public ACharacter, public IToPlayerInterface, public IInputPlayerInterface
{
	GENERATED_BODY()

public:
	AAPlayerCharacter();

protected:
	virtual void BeginPlay() override;
	virtual void SetNickname_Implementation(const FString& Nickname) override;
	virtual void SetVisibilityButtonInteract_Implementation(UWidgetComponent* WidgetComponent,
	                                                        bool bVisibility) override;
	virtual void HandleInteract_Implementation() override;

	UPROPERTY(BlueprintReadWrite, Category = "Player")
	APlayerController* BP_PlayerController;

	UPROPERTY(BlueprintReadWrite, Category = "Player")
	APlayerState* BP_PlayerState;
};