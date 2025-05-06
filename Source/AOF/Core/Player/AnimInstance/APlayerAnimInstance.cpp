// Fill out your copyright notice in the Description page of Project Settings.

#include "KismetAnimationLibrary.h"
#include "APlayerAnimInstance.h"

#include "AOF/Core/Player/Interfaces/ToPlayer/ToPlayerInterface.h"
#include "AOF/Core/Weapon/Interface/ToWeapon/ToWeaponInterface.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetMathLibrary.h"

void UAPlayerAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();

	if (ACharacter* PC = Cast<ACharacter>(TryGetPawnOwner()))
	{
		PlayerCharacter = PC;
		SkeletalMeshCharacter = PC->GetComponentByClass<USkeletalMeshComponent>();
		CharacterMovement = PC->GetCharacterMovement();
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

void UAPlayerAnimInstance::OnNotifyTriggered(const ENotifyType Notify)
{
	switch (Notify)
	{
	case ENotifyType::Drop_Magazine:
		NotifyDropMagazine();
		break;
	case ENotifyType::Place_Magazine:
		NotifyPlaceMagazine();
		break;
	case ENotifyType::Take_Magazine:
		NotifyTakeMagazine();
		break;
	case ENotifyType::Remove_Magazine:
		NotifyRemoveMagazine();
		break;	
	}
}

void UAPlayerAnimInstance::InitializeVariables()
{
	ItemInHand = IToPlayerInterface::Execute_GetItemInHand(PlayerCharacter.Get());
	bIsFalling = CharacterMovement.Get()->IsFalling();
	bIsCrouch = CharacterMovement.Get()->IsCrouching();
	
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
	FRotator PlayerControlRotationSync = IToPlayerInterface::Execute_GetControlRotationSync(PlayerCharacter.Get());
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
	if (ItemInHand.IsValid() && SkeletalMeshCharacter.IsValid() && ItemInHand.Get()->Implements<UToWeaponInterface>())
	{
		USkeletalMeshComponent* WeaponSkeletalMeshComponent = IToWeaponInterface::Execute_GetWeaponSkeletalMeshComponent(ItemInHand.Get());
		if (WeaponSkeletalMeshComponent)
		{
			FVector OutPosition;
			FRotator OutRotation;
			
			FTransform IK_Hand_L = WeaponSkeletalMeshComponent->GetSocketTransform(
				FName("IK_Hand_L"),
				RTS_World);
			
			SkeletalMeshCharacter.Get()->TransformToBoneSpace(
				FName("hand_r"),
				IK_Hand_L.GetLocation(),
				IK_Hand_L.Rotator(),
				OutPosition,
				OutRotation);
			
			LeftHandIKTransform = FTransform(OutRotation, OutPosition, FVector(1.0f, 1.0f, 1.0f));
		}
	}
}

UStaticMeshComponent* UAPlayerAnimInstance::DetachMagazine()
{
	if (ItemInHand.IsValid())
	{
		if (auto MagazineMesh = IToWeaponInterface::Execute_GetMagazineMeshComponent(ItemInHand.Get()))
		{
			MagazineMesh->DetachFromComponent(FDetachmentTransformRules::KeepWorldTransform);
			return MagazineMesh;
		}
	}
	
	return nullptr;
}

void UAPlayerAnimInstance::NotifyDropMagazine()
{
	if (auto MagazineMesh = DetachMagazine())
	{
		MagazineMesh->SetSimulatePhysics(true);
	}
}

void UAPlayerAnimInstance::NotifyPlaceMagazine()
{
	if (auto MagazineMesh = DetachMagazine(); auto WeaponMesh = IToWeaponInterface::Execute_GetWeaponSkeletalMeshComponent(ItemInHand.Get()))
	{
		MagazineMesh->AttachToComponent(
			WeaponMesh,
			FAttachmentTransformRules::SnapToTargetNotIncludingScale,
			FName("mag"));
	}
}

void UAPlayerAnimInstance::NotifyTakeMagazine()
{
	if (PlayerCharacter.IsValid())
	{
		IToPlayerInterface::Execute_TakeMagazine(PlayerCharacter.Get());
	}
}

void UAPlayerAnimInstance::NotifyRemoveMagazine()
{
	if (auto MagazineMesh = DetachMagazine(); SkeletalMeshCharacter.IsValid())
	{
		MagazineMesh->AttachToComponent(
			SkeletalMeshCharacter.Get(),
			FAttachmentTransformRules::SnapToTargetNotIncludingScale,
			FName("skt_Mag"));
	}
}
