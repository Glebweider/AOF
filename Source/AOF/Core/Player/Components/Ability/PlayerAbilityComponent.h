// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "PlayerAbilityComponent.generated.h"


UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class AOF_API UPlayerAbilityComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	/** Getters and Setters */
	float GetHealth() const { return Health; }
	virtual void TakeDamage(float Damage, AActor* Character);

	/** Client RPCs */
	UFUNCTION(Client, Reliable)
	void Client_PlayerDie();
	
	UFUNCTION(Client, Reliable)
	void Client_Sprint(bool bIsNewSprint);

	/** Server RPCs */
	UFUNCTION(Server, Reliable)
	void Server_PlayerDie();

	UFUNCTION(Server, Reliable)
	void Server_Sprint(bool bIsNewSprint);
	
protected:
	virtual void BeginPlay() override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	
	virtual void UpdateStamina();
	virtual void UpdateStaminaRegen();
	
	/** Multicast RPCs */
	UFUNCTION(NetMulticast, Reliable)
	void Multi_PlayerDie();
	
	UFUNCTION(NetMulticast, Reliable)
	void Multi_Sprint(bool bIsNewSprint);

	/** References */
	UPROPERTY()
	ACharacter* PlayerCharacter = nullptr;

	UPROPERTY()
	APlayerController* PlayerController = nullptr;

	UPROPERTY()
	UCharacterMovementComponent* CharacterMovement = nullptr;

	UPROPERTY(EditAnywhere, Category = "UI")
	TSubclassOf<UUserWidget> DeadScreenWidgetClass;

private:
	/** Timer Handles */
	FTimerHandle StaminaSpendTimerHandle;
	FTimerHandle StaminaRegenTimerHandle;
	
	bool bIsSprint;
	bool bIsRegenStamina;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PlayerAbility", meta = (AllowPrivateAccess = "true"))
	float MaxHealth = 100.0f;

	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadOnly, Category = "PlayerAbility", meta = (AllowPrivateAccess = "true"))
	float Health = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PlayerAbility", meta = (AllowPrivateAccess = "true"))
	float MaxStamina = 100.0f;
	
	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadOnly, Category = "PlayerAbility", meta = (AllowPrivateAccess = "true"))
	float Stamina = 0.0f;

};