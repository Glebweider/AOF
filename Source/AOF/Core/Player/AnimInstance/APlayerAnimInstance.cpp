// Fill out your copyright notice in the Description page of Project Settings.

#include "KismetAnimationLibrary.h"
#include "APlayerAnimInstance.h"

#include "AOF/Core/Weapon/Interface/ToWeapon/ToWeaponInterface.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetMathLibrary.h"

UAPlayerAnimInstance::UAPlayerAnimInstance()
{
}

void UAPlayerAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();

	if (AAPlayerCharacter* PC = Cast<AAPlayerCharacter>(TryGetPawnOwner()))
	{
		PlayerCharacter = PC;
		CharacterMovement = PlayerCharacter->GetCharacterMovement();
	}
}

void UAPlayerAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);
	
	if (PlayerCharacter.IsValid() && CharacterMovement.IsValid())
	{
		InitializeVariables();
		SnapToWeaponLeftHand();
	}
}

void UAPlayerAnimInstance::InitializeVariables()
{
	ItemInHand = PlayerCharacter.Get()->GetItemInHand();
	bIsFalling = CharacterMovement.Get()->IsFalling();
	
	InitSpeedDirection();
	InitPitch();
}

void UAPlayerAnimInstance::InitSpeedDirection()
{
	FVector PlayerVelocity = PlayerCharacter.Get()->GetVelocity();
	FRotator PlayerRotation = PlayerCharacter.Get()->GetActorRotation();

	Speed = PlayerVelocity.Length();
	Direction = UKismetAnimationLibrary::CalculateDirection(PlayerVelocity, PlayerRotation);

	float MaxWalkSpeed = CharacterMovement.Get()->MaxWalkSpeed > 0 ? CharacterMovement.Get()->MaxWalkSpeed : 600;
	bIsSprint = Speed >= MaxWalkSpeed - 5;
	bIsShouldMove = Speed > 3 && CharacterMovement.Get()->GetCurrentAcceleration() != FVector::ZeroVector;
}

void UAPlayerAnimInstance::InitPitch()
{
	FRotator PlayerControlRotationSync = PlayerCharacter.Get()->GetControlRotationSync();
	AimRotator = PlayerControlRotationSync;

	FRotator PlayerDeltaRotator = UKismetMathLibrary::NormalizedDeltaRotator(
		PlayerControlRotationSync,
		PlayerCharacter.Get()->GetActorRotation());
	FRotator PlayerRInterpToRotator = UKismetMathLibrary::RInterpTo(
		FRotator(Pitch, 0.f, 0.f),
		PlayerDeltaRotator,
		GetWorld()->GetDeltaSeconds(),
		10.f);

	Pitch = UKismetMathLibrary::ClampAngle(
		PlayerRInterpToRotator.Pitch,
		-90.f,
		90.f);

	if (ItemInHand.IsValid() && ItemInHand.Get()->Implements<UToWeaponInterface>())
	{
		bool bIsPlayerReloading = IToWeaponInterface::Execute_GetIsReloading(ItemInHand.Get());
		LeftHandIKAlpha = UKismetMathLibrary::Lerp(
			LeftHandIKAlpha,
			bIsPlayerReloading ? 0.f : 1.f,
			0.3f);
	}
}

void UAPlayerAnimInstance::SnapToWeaponLeftHand()
{
	if (ItemInHand.IsValid() && ItemInHand.Get()->Implements<UToWeaponInterface>())
	{
		USkeletalMeshComponent* WeaponSkeletalMeshComponent = IToWeaponInterface::Execute_GetWeaponSkeletalMeshComponent(ItemInHand.Get());
		if (WeaponSkeletalMeshComponent)
		{
			FVector OutPosition;
			FRotator OutRotation;
			
			FTransform IK_Hand_L = WeaponSkeletalMeshComponent->GetSocketTransform(
				FName("IK_Hand_L"),
				RTS_World);
			
			PlayerCharacter.Get()->GetSkeletalMeshComponent()->TransformToBoneSpace(
				FName("hand_r"),
				IK_Hand_L.GetLocation(),
				IK_Hand_L.Rotator(),
				OutPosition,
				OutRotation);
			
			LeftHandIKTransform = FTransform(OutRotation, OutPosition, FVector(1.0f, 1.0f, 1.0f));
		}
	}
}