// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AOF/Core/Inventory/Component/Inventory/InventoryComponent.h"
#include "AOF/Core/Player/Components/Ability/PlayerAbilityComponent.h"
#include "AOF/Core/Player/Components/UI/PlayerUIComponent.h"
#include "AOF/Core/Player/Interfaces/InputPlayer/InputPlayerInterface.h"
#include "AOF/Core/Player/Interfaces/ToPlayer/ToPlayerInterface.h"
#include "AOF/Core/Weapon/Interface/Damage/DamageInterface.h"
#include "Components/SphereComponent.h"
#include "GameFramework/Character.h"
#include "GameFramework/PlayerController.h"
#include "APlayerCharacter.generated.h"

UCLASS()
class AOF_API AAPlayerCharacter : public ACharacter, public IToPlayerInterface, public IInputPlayerInterface, public IDamageInterface
{
	GENERATED_BODY()

public:
	AAPlayerCharacter();
	
	/** Server RPCs */
	UFUNCTION(Server, Reliable)
	void Server_Interact(AActor* ItemPickUp, FInventoryItem InventoryItemPickUp);

	UFUNCTION(Server, Reliable, BlueprintCallable)
	void Server_TakeMagazine();
	
	UFUNCTION(Server, Reliable)
	void Server_Crouch(bool bIsNewCrouch);

	/** Interfaces */
	virtual void SetNickname_Implementation(const FString& Nickname) override;
	virtual void SetVisibilityUIWidget_Implementation(UWidgetComponent* WidgetComponent, bool bVisibility) override;
	virtual void PickUpItem_Implementation(AActor* ItemPickUp, FInventoryItem InventoryItemPickUp) override { Server_Interact(ItemPickUp, InventoryItemPickUp); };
	virtual void TakeDamage_Implementation(float Damage, AActor* Character) override { PlayerAbilityComponent->TakeDamage(Damage, Character); };
	virtual void TakeMagazine_Implementation() override { Server_TakeMagazine(); };
	virtual AActor* GetItemInHand_Implementation() override { return InventoryComponent ? InventoryComponent->SelectedItemInHand : nullptr; };
	virtual FRotator GetControlRotationSync_Implementation() override { return ControlRotationSync; };

	/** Interfaces PlayerInput */
	virtual void HandleInteract_Implementation() override;
	virtual void HandleInventory_Implementation() override;
	virtual void HandleCrouch_Implementation(bool bIsNewCrouch) override { Server_Crouch(bIsNewCrouch); };
	virtual void HandleSprint_Implementation(bool bIsSprint) override { PlayerAbilityComponent->Client_Sprint(bIsSprint); };

protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	
	/** Multicast RPCs */
	UFUNCTION(NetMulticast, Reliable)
	void Multi_Interact(AActor* ItemPickUp, FInventoryItem InventoryItemPickUp);

	UFUNCTION(NetMulticast, Reliable)
	void Multi_TakeMagazine();

	UFUNCTION(NetMulticast, Reliable)
	void Multi_Crouch(bool bIsNewCrouch);

	/** Helpers */
	bool AddItemToInventory(AActor* ItemPickUp, FInventoryItem InventoryItemPickUp);
	void SetUIActive(bool bIsActive, bool bIsIgnoreMove, bool bIsIgnoreLook);
	bool CanCreateUI();

	/** UI */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI")
	TSubclassOf<UUserWidget> WidgetPlayerHUD;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "UI")
	UWidgetComponent* PlayerNameTagComponent;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Collision")
	USphereComponent* DetectionSphere;

	UPROPERTY(Replicated, BlueprintReadWrite, Category = "Player")
	FRotator ControlRotationSync;
	
	UPROPERTY(BlueprintReadWrite, Category = "Player")
	APlayerController* BP_PlayerController;

	UPROPERTY(BlueprintReadWrite, Category = "Player")
	APlayerState* BP_PlayerState;
	
	UPROPERTY()
	USkeletalMeshComponent* SkeletalMeshComponent = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UInventoryComponent* InventoryComponent;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UPlayerAbilityComponent* PlayerAbilityComponent;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UPlayerUIComponent* PlayerUIComponent;

	UFUNCTION()
	void OnBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, 
						UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, 
						bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void OnEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, 
					  UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

private:
	bool bIsInventoryOpen = false;
};