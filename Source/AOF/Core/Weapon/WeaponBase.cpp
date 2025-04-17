// Fill out your copyright notice in the Description page of Project Settings.


#include "WeaponBase.h"

#include "AOF/Core/Functions/BPF_Functions.h"
#include "AOF/Core/Player/Interfaces/ToPlayer/ToPlayerInterface.h"
#include "Engine/AssetManager.h"
#include "Interface/Damage/DamageInterface.h"
#include "Kismet/KismetMathLibrary.h"
#include "Net/UnrealNetwork.h"
#include "Structures/WeaponStructure.h"

AWeaponBase::AWeaponBase()
{
	PrimaryActorTick.bCanEverTick = false;
}


void AWeaponBase::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);

	
	if (DataTableWeapon.IsNull() && !RowName.IsValid()) return;
	if (const auto WeaponDataRow = DataTableWeapon.DataTable->FindRow<FWeaponStruct>(RowName, TEXT("Loading Weapon Data")))
	{
		WeaponData.Damage = WeaponDataRow->Damage;
		WeaponData.FireSpeed = 60.0f / WeaponDataRow->FireSpeed;
		WeaponData.BulletInShoot = WeaponDataRow->BulletInShoot;
		WeaponData.AmmoMagazine = WeaponDataRow->AmmoMagazine;
		WeaponData.MaxAmmoMagazine = WeaponDataRow->MaxAmmoMagazine;
		
		WeaponData.HeadMultiplier = WeaponDataRow->HeadMultiplier;
		WeaponData.LegsMultiplier = WeaponDataRow->LegsMultiplier;
		WeaponData.RecoilMultiplier = WeaponDataRow->RecoilMultiplier;
		
		WeaponData.FireType = WeaponDataRow->FireType;
		WeaponData.AmmoType = WeaponDataRow->AmmoType;
		
		WeaponData.Name = WeaponDataRow->Name;
		
		WeaponData.WeaponMesh = WeaponDataRow->WeaponMesh;
		WeaponData.MagazineMesh = WeaponDataRow->MagazineMesh;
		WeaponData.MuzzleSound = WeaponDataRow->MuzzleSound;
		WeaponData.MuzzleEmitter = WeaponDataRow->MuzzleEmitter;
		WeaponData.ReloadCharacterMontage = WeaponDataRow->ReloadCharacterMontage;
		
		CurrentAmmo = WeaponDataRow->AmmoMagazine;

		SkeletalMeshComponent = FindComponentByClass<USkeletalMeshComponent>();
		if (IsValid(SkeletalMeshComponent))
		{
			const FSoftObjectPath WeaponMeshPath = WeaponData.WeaponMesh.ToSoftObjectPath();
			if (!WeaponMeshPath.IsValid()) return;

			UAssetManager::GetStreamableManager().RequestAsyncLoad(WeaponMeshPath, [this]()
			{
				if (WeaponData.WeaponMesh.IsValid() && SkeletalMeshComponent)
				{
					SkeletalMeshComponent->SetSkeletalMeshAsset(WeaponData.WeaponMesh.Get());
				}
			});
		}

		ItemData.ItemID = WeaponData.Name;
		ItemData.Quantity = 0;
		ItemData.ItemClass = this->GetClass();
	}
}


void AWeaponBase::BeginPlay()
{
	Super::BeginPlay();

	
	AActor* OwnerActor = GetOwner();
	if (!OwnerActor) return;
	
	Character = OwnerActor;
	
	SkeletalMeshComponent->HideBoneByName("Mag_low", PBO_None);

	const FSoftObjectPath MagazinMeshPath = WeaponData.MagazineMesh.ToSoftObjectPath();
	if (!MagazinMeshPath.IsValid()) return;

	UAssetManager::GetStreamableManager().RequestAsyncLoad(MagazinMeshPath, [this]()
	{
		if (WeaponData.WeaponMesh.IsValid() && SkeletalMeshComponent)
		{
			UStaticMeshComponent* MagMeshComponent = NewObject<UStaticMeshComponent>(this, UStaticMeshComponent::StaticClass(), FName("Mag"));
			if (!MagMeshComponent) return;

			MagMeshComponent->RegisterComponent();
			MagMeshComponent->SetStaticMesh(WeaponData.MagazineMesh.Get());
			MagMeshComponent->AttachToComponent(SkeletalMeshComponent, FAttachmentTransformRules::KeepRelativeTransform, FName("Mag"));
		}
	});
	
	/*
	AController* OwnerController = Cast<AController>(Character);
	if (OwnerController == nullptr)
	{
		OwnerController = Cast<AAIcontroller>(Character->Controller);
	}
	*/
}

void AWeaponBase::InteractItem_Implementation(AActor* CharacterInteract)
{
	IToPlayerInterface::Execute_PickUpItem(CharacterInteract, this,ItemData);
}

void AWeaponBase::UseItem_Implementation()
{
	bIsFire = true;

	if (!CanFire()) return;
	switch (WeaponData.FireType)
	{
		case EFireType::Auto:
			AutoFire();
			break;

		case EFireType::Burst:
			BurstFire();
			break;

		case EFireType::Single:
			Fire();
			AutoReload();
			break;
	}
}

void AWeaponBase::StopUseItem_Implementation()
{
	bIsFire	= false;
	GetWorld()->GetTimerManager().ClearTimer(FireTimerHandle);
}

void AWeaponBase::AutoFire()
{
	if (!bIsFire || !CanFire()) return;
	UE_LOG(LogTemp, Warning, TEXT("FIRE SPEED %f"), WeaponData.FireSpeed);
	
	Fire();
	if (CanFire())
	{
		GetWorld()->GetTimerManager().SetTimer(
			FireTimerHandle,
			this,
			&AWeaponBase::AutoFire,
			WeaponData.FireSpeed,
			true
		);
	}
	else
	{
		AutoReload();
	}
}

void AWeaponBase::BurstFire()
{
	if (!CanFire()) return;
	if (CurrentBulletsInBurst < 3)
	{
		CurrentBulletsInBurst++;
		Fire();

		if (CanFire())
		{
			GetWorld()->GetTimerManager().SetTimer(
				FireTimerHandle,
				this,
				&AWeaponBase::BurstFire,
				WeaponData.FireSpeed,
				true
			);
		}
		else
		{
			AutoReload();
		}
	}
	else
	{
		CurrentBulletsInBurst = 0;
	}
}

void AWeaponBase::AutoReload()
{
	UE_LOG(LogTemp, Warning, TEXT("AutoReload"));
}

bool AWeaponBase::CanFire()
{
	UE_LOG(LogTemp, Warning, TEXT("CanFire %d"), CurrentAmmo);
	return bIsFire && !bIsShooting && CurrentAmmo > 0;
}

void AWeaponBase::Fire() {
	FVector StartDirection;
	FVector EndDirection;
	
	UBPF_Functions::GetLookDirection(200000.0f, this, StartDirection, EndDirection);
	Server_Fire(StartDirection, EndDirection);
}

void AWeaponBase::Server_Fire_Implementation(FVector StartDirection, FVector EndDirection)
{
	SpawnProjectile(StartDirection, EndDirection);
	Multi_Fire();
}

void AWeaponBase::SpawnProjectile(FVector& StartDirection, FVector& EndDirection)
{
	CurrentAmmo--;
	
	if (!SkeletalMeshComponent) return;
	FVector MuzzleLocation = SkeletalMeshComponent->GetSocketLocation(FName("Muzzle"));
	
	FHitResult HitResult;
	FVector TargetLocation;
	
	FCollisionQueryParams CollisionParams;
	CollisionParams.AddIgnoredActor(this);
	CollisionParams.AddIgnoredActor(GetOwner());

	const bool bHitLineTrace = GetWorld()->LineTraceSingleByChannel(HitResult, StartDirection, EndDirection, ECC_Camera, CollisionParams);
	
#if WITH_EDITOR
	if (bHitLineTrace)
	{
		DrawDebugLine(GetWorld(), StartDirection, EndDirection, FColor::Green, false, 1, 0, 1);
	}
	else
	{
		DrawDebugLine(GetWorld(), StartDirection, EndDirection, FColor::Red, false, 1, 0, 1);
	}
#endif
	if (bHitLineTrace)
	{
		TargetLocation = HitResult.Location;
	}
	TargetLocation = HitResult.TraceEnd;
	FVector EndLocation = MuzzleLocation + UKismetMathLibrary::FindLookAtRotation(MuzzleLocation, TargetLocation).Vector() * 10000;
	
	for (int32 i = 0; i < WeaponData.BulletInShoot; i++)
	{
		if (i != 0)
		{
			const bool bHitSphereTrace = GetWorld()->SweepSingleByChannel(
				HitResult,
				MuzzleLocation,
				EndLocation,
				FQuat::Identity,
				ECC_Visibility,
				FCollisionShape::MakeSphere(4.0f),
				CollisionParams);
			
			Multi_Fire_Recoil();
			if (bHitSphereTrace)
			{
				if (HitResult.Component->GetMaterial(0))
				{
					Multi_Spawn_Hit();
				}

				if (HitResult.GetActor()->Implements<UDamageInterface>())
				{
					IDamageInterface::Execute_TakeDamage(HitResult.GetActor(), 30, GetOwner());
				}
			}
		}
	}
}

void AWeaponBase::Multi_Fire_Implementation()
{
	UE_LOG(LogTemp, Warning, TEXT("Multi_Fire"));
}

void AWeaponBase::Multi_Fire_Recoil_Implementation()
{

}

void AWeaponBase::Multi_Spawn_Hit_Implementation()
{

}
void AWeaponBase::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(AWeaponBase, CurrentAmmo);
}
