// Fill out your copyright notice in the Description page of Project Settings.


#include "WeaponBase.h"

#include "VectorUtil.h"
#include "AOF/Core/Functions/BPF_Functions.h"
#include "AOF/Core/Player/Interfaces/ToPlayer/ToPlayerInterface.h"
#include "Engine/AssetManager.h"
#include "GameFramework/Character.h"
#include "Interface/Damage/DamageInterface.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "Net/UnrealNetwork.h"
#include "Particles/ParticleSystem.h"
#include "Sound/SoundCue.h"
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
					USkeletalMesh* LoadedMesh = WeaponData.WeaponMesh.Get();
					SkeletalMeshComponent->SetSkeletalMeshAsset(LoadedMesh);
					
					if (LoadedMesh->GetRefSkeleton().FindBoneIndex("Mag_low") != INDEX_NONE)
					{
						SkeletalMeshComponent->HideBoneByName(FName("Mag_low"), PBO_None);
					}
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

	if (CanFire())
	{
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
	} else
	{
		AutoReload();
	}
}

void AWeaponBase::StopUseItem_Implementation()
{
	bIsFire = false;
	
	if (!bIsShooting)
	{
		GetWorld()->GetTimerManager().ClearTimer(FireTimerHandle);
	}
}

void AWeaponBase::AutoFire()
{
	if (bIsReloading) return;
	if (CanFire())
	{
		Fire();
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
				false
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
	if (CurrentAmmo == 0 && CurrentAmmo != WeaponData.AmmoMagazine && !bIsReloading && WeaponData.MaxAmmoMagazine != 0)
	{
		Server_Reload();
	}
}

bool AWeaponBase::CanFire()
{
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
	FCollisionQueryParams CollisionParams;
	CollisionParams.AddIgnoredActor(this);
	CollisionParams.AddIgnoredActor(GetOwner());

	const bool bHitLineTrace = GetWorld()->LineTraceSingleByChannel(HitResult, StartDirection, EndDirection, ECC_Camera, CollisionParams);
	
	FVector Direction = UKismetMathLibrary::FindLookAtRotation(MuzzleLocation, bHitLineTrace ? HitResult.Location : HitResult.TraceEnd).Vector();
	FVector EndLocation = MuzzleLocation + Direction * 10000;
	
	for (int32 i = 0; i < WeaponData.BulletInShoot; i++)
	{
		
		FHitResult SphereHit;
		const bool bHitSphere = GetWorld()->SweepSingleByChannel(
			SphereHit,
			MuzzleLocation,
			EndLocation,
			FQuat::Identity,
			ECC_Camera,
			FCollisionShape::MakeSphere(4.0f),
			CollisionParams);
			
#if WITH_EDITOR
			FColor LineColor = bHitSphere ? FColor::Green : FColor::Red;
			DrawDebugLine(GetWorld(), MuzzleLocation, EndLocation, LineColor, false, 2.f, 0, 1.f);
			
			if (bHitSphere)
			{
				DrawDebugSphere(GetWorld(), SphereHit.Location, 8.f, 12, FColor::Yellow, false, 2.f);
			}
#endif
			
		Multi_Fire_Recoil();
		if (bHitSphere)
		{
			if (SphereHit.Component.IsValid() && SphereHit.Component->GetMaterial(0))
			{
				Multi_Spawn_Hit(UPhysicalMaterial::DetermineSurfaceType(SphereHit.PhysMaterial.Get()), SphereHit.Location, SphereHit.Component.Get());
			}
			
			if (SphereHit.GetActor()->Implements<UDamageInterface>())
			{

				// Damage Calculate
					
				IDamageInterface::Execute_TakeDamage(SphereHit.GetActor(), 30, GetOwner());
			}
		}
	}
}

void AWeaponBase::Multi_Fire_Implementation()
{
	if (!LoadedMuzzleEmitter || !LoadedMuzzleSound)
	{
		TArray<FSoftObjectPath> AssetsToLoad;

		if (WeaponData.MuzzleSound.ToSoftObjectPath().IsValid())
		{
			AssetsToLoad.Add(WeaponData.MuzzleSound.ToSoftObjectPath());
		}
		if (WeaponData.MuzzleEmitter.ToSoftObjectPath().IsValid())
		{
			AssetsToLoad.Add(WeaponData.MuzzleEmitter.ToSoftObjectPath());
		}

		FStreamableManager& Streamable = UAssetManager::GetStreamableManager();
		Streamable.RequestAsyncLoad(AssetsToLoad, FStreamableDelegate::CreateWeakLambda(this, [this]()
		{
			if (WeaponData.MuzzleSound.IsValid())
			{
				LoadedMuzzleSound = Cast<USoundCue>(WeaponData.MuzzleSound.Get());
			}
			if (WeaponData.MuzzleEmitter.IsValid())
			{
				LoadedMuzzleEmitter = Cast<UParticleSystem>(WeaponData.MuzzleEmitter.Get());
			}
		}));
	}
	
	if (SkeletalMeshComponent)
	{
		if (LoadedMuzzleEmitter)
		{
			UGameplayStatics::SpawnEmitterAttached(
				LoadedMuzzleEmitter,
				SkeletalMeshComponent,
				FName("Muzzle"),
				FVector::ZeroVector,
				FRotator::ZeroRotator,
				EAttachLocation::SnapToTarget,
				true
			);
		}

		if (LoadedMuzzleSound)
		{
			UGameplayStatics::SpawnSoundAttached(
				LoadedMuzzleSound,
				SkeletalMeshComponent,
				FName("Muzzle"),
				FVector::ZeroVector,
				EAttachLocation::SnapToTarget,
				true
			);
		}
	}
}

void AWeaponBase::Server_Reload_Implementation()
{
	bIsReloading = true;
	Multi_Reload();
}

void AWeaponBase::Multi_Reload_Implementation()
{
    if (IsValid(LoadedReloadCharacterMontage))
    {
    	if (UAnimInstance* AnimInstance = Cast<ACharacter>(GetOwner())->GetMesh()->GetAnimInstance())
    	{
    		AnimInstance->Montage_Play(LoadedReloadCharacterMontage);
    		
            float Duration = LoadedReloadCharacterMontage->GetPlayLength();
            if (Duration <= 0.f) Duration = 1.f;

                FTimerHandle TimerHandle;
                GetWorld()->GetTimerManager().SetTimer(
                    TimerHandle,
                    this,
                    &AWeaponBase::ReloadAfterDelay,
                    Duration,
                    false
                );}
        return;
    }
	
    const FSoftObjectPath MontagePath = WeaponData.ReloadCharacterMontage.ToSoftObjectPath();
    if (!MontagePath.IsValid()) return;
	
    UAssetManager::GetStreamableManager().RequestAsyncLoad(
        MontagePath,
        [this]()
        {
            if (WeaponData.ReloadCharacterMontage.IsValid())
            {
                LoadedReloadCharacterMontage = WeaponData.ReloadCharacterMontage.Get();

                if (UAnimInstance* AnimInstance = Cast<ACharacter>(GetOwner())->GetMesh()->GetAnimInstance())
                {
                    AnimInstance->Montage_Play(LoadedReloadCharacterMontage);
                    	
                    float Duration = LoadedReloadCharacterMontage->GetPlayLength();
                    if (Duration <= 0.f) Duration = 1.f;
                    	
                    FTimerHandle TimerHandle;
                    GetWorld()->GetTimerManager().SetTimer(
                        TimerHandle,
                        this,
                        &AWeaponBase::ReloadAfterDelay,
                        Duration,
                        false
                    );
                }
            }
        }
    );
}

void AWeaponBase::ReloadAfterDelay()
{
	CalculateAmmo();
	bIsReloading = false;
}

void AWeaponBase::CalculateAmmo()
{
	int32 TempAmmo = WeaponData.MaxAmmoMagazine;
		
	WeaponData.MaxAmmoMagazine = UE::Geometry::VectorUtil::Clamp(WeaponData.MaxAmmoMagazine - (WeaponData.AmmoMagazine - CurrentAmmo), 0, WeaponData.MaxAmmoMagazine);
	CurrentAmmo = TempAmmo - WeaponData.MaxAmmoMagazine + CurrentAmmo;
}

void AWeaponBase::Multi_Fire_Recoil_Implementation()
{

}

void AWeaponBase::Multi_Spawn_Hit_Implementation(EPhysicalSurface SurfaceType, FVector Location, UPrimitiveComponent* Component)
{
	USoundBase* HitSound = nullptr;
	UParticleSystem* HitEmitter = nullptr;
	UMaterialInterface* HitDecal = nullptr;

	/*switch (SurfaceType)
	{
		case SURFACE_FLESHDEFAULT:
			HitSound = FleshSound;
			HitEmitter = FleshEmitter;
			HitDecal = FleshDecal;
			break;
		case SURFACE_METAL:
			HitSound = MetalSound;
			HitEmitter = MetalEmitter;
			HitDecal = MetalDecal;
			break;
		default:
			HitSound = DefaultSound;
			HitEmitter = DefaultEmitter;
			HitDecal = DefaultDecal;
			break;
	}
	
	if (HitSound)
	{
		UGameplayStatics::PlaySoundAtLocation(GetWorld(), HitSound, Location);
	}
	
	if (HitEmitter)
	{
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), HitEmitter, Location, FRotator::ZeroRotator, FVector(1.0f), true);
	}
	
	if (HitDecal && Component)
	{
		UGameplayStatics::SpawnDecalAttached(
			HitDecal,
			FVector(20.0f),
			Component,
			NAME_None,
			Location,
			FRotator::ZeroRotator,
			EAttachLocation::KeepRelativeOffset,
			10.0f
		);
	}
	*/
}

void AWeaponBase::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(AWeaponBase, CurrentAmmo);
	DOREPLIFETIME(AWeaponBase, bIsReloading);
}
