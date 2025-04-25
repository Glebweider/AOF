// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AOF/Core/Inventory/Component/Inventory/InventoryComponent.h"
#include "AOF/Core/Player/Components/Ability/PlayerAbilityComponent.h"
#include "AOF/Core/Player/Interfaces/InputPlayer/InputPlayerInterface.h"
#include "AOF/Core/Player/Interfaces/ToPlayer/ToPlayerInterface.h"
#include "AOF/Core/Weapon/Interface/Damage/DamageInterface.h"
#include "GameFramework/Character.h"
#include "GameFramework/PlayerController.h"
#include "APlayerCharacter.generated.h"

UCLASS()
class AOF_API AAPlayerCharacter : public ACharacter, public IToPlayerInterface, public IInputPlayerInterface, public IDamageInterface
{
	GENERATED_BODY()

public:
	AAPlayerCharacter();
	
	UFUNCTION(Server, Reliable)
	void Server_Interact(AActor* ItemPickUp, FInventoryItem InventoryItemPickUp);

	UFUNCTION(Server, Reliable, BlueprintCallable)
	void Server_TakeMagazine();

protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	void Multi_TakeMagazine_Implementation();
	bool AddItemToInventory(AActor* ItemPickUp, FInventoryItem InventoryItemPickUp);

	virtual void SetNickname_Implementation(const FString& Nickname) override;
	virtual void SetVisibilityButtonInteract_Implementation(UWidgetComponent* WidgetComponent, bool bVisibility) override;
	virtual void HandleInteract_Implementation() override;
	virtual void PickUpItem_Implementation(AActor* ItemPickUp, FInventoryItem InventoryItemPickUp) override;
	virtual void TakeDamage_Implementation(float Damage, AActor* Character) override;

	UFUNCTION(NetMulticast, Reliable)
	void Multicast_Interact(AActor* ItemPickUp, FInventoryItem InventoryItemPickUp);

	UFUNCTION(NetMulticast, Reliable)
	void Multi_TakeMagazine();
	
	UPROPERTY(Replicated, BlueprintReadWrite, Category = "Player")
	bool bIsCrouch;

	UPROPERTY(Replicated, BlueprintReadWrite, Category = "Player")
	FRotator ControlRotationSync;
	
	UPROPERTY(BlueprintReadWrite, Category = "Player")
	APlayerController* BP_PlayerController;

	UPROPERTY()
	USkeletalMeshComponent* SkeletalMeshComponent = nullptr;

	UPROPERTY(BlueprintReadWrite, Category = "Player")
	APlayerState* BP_PlayerState;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UInventoryComponent* InventoryComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UPlayerAbilityComponent* PlayerAbilityComponent;
};