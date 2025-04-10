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
	
	UFUNCTION(Server, Reliable)
	void Server_Interact(AActor* ItemPickUp, FInventoryItem InventoryItemPickUp);

protected:
	virtual void BeginPlay() override;

	bool AddItemToInventory(AActor* ItemPickUp, FInventoryItem InventoryItemPickUp);

	virtual void SetNickname_Implementation(const FString& Nickname) override;
	virtual void SetVisibilityButtonInteract_Implementation(UWidgetComponent* WidgetComponent, bool bVisibility) override;
	virtual void HandleInteract_Implementation() override;
	virtual void PickUpItem_Implementation(AActor* ItemPickUp, FInventoryItem InventoryItemPickUp) override;

	UFUNCTION(NetMulticast, Reliable)
	void Multicast_Interact(AActor* ItemPickUp, FInventoryItem InventoryItemPickUp);

	UPROPERTY(BlueprintReadWrite, Category = "Player")
	APlayerController* BP_PlayerController;

	UPROPERTY(BlueprintReadWrite, Category = "Player")
	APlayerState* BP_PlayerState;
};