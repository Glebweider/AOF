// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "PlayerAbilityComponent.generated.h"


UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class AOF_API UPlayerAbilityComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UPlayerAbilityComponent();

	float GetHealth() const { return Health; }
	virtual void TakeDamage(float Damage, AActor* Character);

protected:
	virtual void BeginPlay() override;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player")
	ACharacter* Player;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player")
	APlayerController* PlayerController;

	UPROPERTY(EditAnywhere, Category = "UI")
	TSubclassOf<UUserWidget> DeadScreenWidgetClass;

private:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PlayerAbility", meta = (AllowPrivateAccess = "true"))
	float MaxHealth = 100.0f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "PlayerAbility", meta = (AllowPrivateAccess = "true"))
	float Health = 0.0f;

	UFUNCTION(Client, Reliable)
	void Client_PlayerDie();

	UFUNCTION(Server, Reliable)
	void Server_PlayerDie();

	UFUNCTION(NetMulticast, Reliable)
	void Multi_PlayerDie();
};