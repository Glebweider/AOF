﻿// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "AOF/Core/Player/Notifies/ENotifyType.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "APlayerAnimInstance.generated.h"

/**
 * 
 */
UCLASS()
class AOF_API UAPlayerAnimInstance : public UAnimInstance
{
	GENERATED_BODY()

public:
	virtual void NativeInitializeAnimation() override;
	virtual void NativeUpdateAnimation(float DeltaSeconds) override;
	virtual void OnNotifyTriggered(const ENotifyType Notify);
	
protected:
	/** Helpers */
	virtual void SnapToWeaponLeftHand();
	virtual void InitializeVariables();
	virtual void InitSpeedDirection();
	virtual void InitPitch();

	/** Notifys */
	virtual UStaticMeshComponent* DetachMagazine();
	virtual void NotifyDropMagazine();
	virtual void NotifyPlaceMagazine();
	virtual void NotifyTakeMagazine();
	virtual void NotifyRemoveMagazine();
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Player Animation")
	FRotator AimRotator;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Player Animation")
	FTransform LeftHandIKTransform;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Player Animation")
	float Pitch;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Player Animation")
	float Speed;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Player Animation")
	float Direction;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Player Animation")
	float LeftHandIKAlpha;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Player Animation")
	bool bIsSprint;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Player Animation")
	bool bIsCrouch;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Player Animation")
	bool bIsShouldMove;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Player Animation")
	bool bIsFalling;

private:
	TWeakObjectPtr<UCharacterMovementComponent> CharacterMovement;
	TWeakObjectPtr<ACharacter> PlayerCharacter;
	TWeakObjectPtr<USkeletalMeshComponent> SkeletalMeshCharacter;
	TWeakObjectPtr<AActor> ItemInHand;
};
