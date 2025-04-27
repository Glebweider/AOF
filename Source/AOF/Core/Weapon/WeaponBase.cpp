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

	
	if (DataTableWeapon.IsNull() || !RowName.IsValid()) return;
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
		WeaponData.FireAnimation = WeaponDataRow->FireAnimation;
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
	
	TArray<FSoftObjectPath> AssetsToLoad;
	if (WeaponData.MagazineMesh.ToSoftObjectPath().IsValid()) { AssetsToLoad.Add(WeaponData.MagazineMesh.ToSoftObjectPath()); }
	if (WeaponData.MuzzleSound.ToSoftObjectPath().IsValid()) { AssetsToLoad.Add(WeaponData.MuzzleSound.ToSoftObjectPath()); }
	if (WeaponData.MuzzleEmitter.ToSoftObjectPath().IsValid()) { AssetsToLoad.Add(WeaponData.MuzzleEmitter.ToSoftObjectPath()); }
	if (WeaponData.FireAnimation.ToSoftObjectPath().IsValid()) { AssetsToLoad.Add(WeaponData.FireAnimation.ToSoftObjectPath()); }
	
	if (AssetsToLoad.Num() == 0) return;
	UAssetManager::GetStreamableManager().RequestAsyncLoad(AssetsToLoad, FStreamableDelegate::CreateWeakLambda(this, [this]()
	{
		if (WeaponData.MagazineMesh.IsValid() && SkeletalMeshComponent)
		{
			MagMeshComponent = NewObject<UStaticMeshComponent>(this, UStaticMeshComponent::StaticClass(), FName("Mag"));
			if (MagMeshComponent)
			{
				MagStaticMesh = WeaponData.MagazineMesh.Get();

				MagMeshComponent->RegisterComponent();
				MagMeshComponent->SetStaticMesh(MagStaticMesh);
				MagMeshComponent->AttachToComponent(SkeletalMeshComponent, FAttachmentTransformRules::KeepRelativeTransform, FName("Mag"));
			}
		}
		
		if (WeaponData.MuzzleSound.IsValid()) { LoadedMuzzleSound = Cast<USoundCue>(WeaponData.MuzzleSound.Get()); }
		if (WeaponData.MuzzleEmitter.IsValid()) { LoadedMuzzleEmitter = Cast<UParticleSystem>(WeaponData.MuzzleEmitter.Get()); }
		if (WeaponData.FireAnimation.IsValid()) { LoadedFireAnimation = Cast<UAnimSequence>(WeaponData.FireAnimation.Get()); }
	}));
}

void AWeaponBase::InteractItem_Implementation(AActor* CharacterInteract)
{
	IToPlayerInterface::Execute_PickUpItem(CharacterInteract, this,ItemData);
}

void AWeaponBase::UseItem_Implementation()
{
	bIsFire = true;

	if (!CanFire())
	{
		AutoReload();
		return;
	}

	switch (WeaponData.FireType)
	{
	case EFireType::Auto:
		AutoFire();
		break;
	case EFireType::Burst:
		BurstFire();
		break;
	case EFireType::Single:
		SingleFire();
		break;
	}
}

void AWeaponBase::StopUseItem_Implementation()
{
	bIsFire = false;
	
	if (!bIsShoot)
	{
		GetWorld()->GetTimerManager().ClearTimer(FireTimerHandle);
	}
}

void AWeaponBase::Fire() {
	FVector StartDirection;
	FVector EndDirection;
	
	UBPF_Functions::GetLookDirection(200000.0f, this, StartDirection, EndDirection);
	Server_Fire(StartDirection, EndDirection);
}

void AWeaponBase::AutoFire()
{
	if (CanFire())
	{
		Fire();
		bIsShoot = true;
		
		GetWorld()->GetTimerManager().SetTimer(
			FireTimerHandle,
			[this]()
			{
				bIsShoot = false;
				AutoFire();
			},
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
	if (CanFire())
	{
		if (CurrentBulletsInBurst < 3)
		{
			Fire();
			
			bIsShoot = true;
			CurrentBulletsInBurst++;
			
			GetWorld()->GetTimerManager().SetTimer(
				FireTimerHandle,
				[this]()
				{
					bIsShoot = false;
					BurstFire();
				},
				WeaponData.FireSpeed,
				false
			);
		}
		else
		{
			CurrentBulletsInBurst = 0;
		}
	}
	else
	{
		CurrentBulletsInBurst = 0;
		AutoReload();
	}
}

void AWeaponBase::SingleFire()
{
	if (CanFire())
	{
		Fire();
		bIsShoot = true;
		
		GetWorld()->GetTimerManager().SetTimer(
			FireTimerHandle,
			[this]()
			{
				bIsShoot = false;
				AutoReload();
			},
			WeaponData.FireSpeed,
			false
		);
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
	return bIsFire && !bIsShoot && CurrentAmmo > 0 && !bIsReloading;
}

void AWeaponBase::Server_Fire_Implementation(FVector StartDirection, FVector EndDirection)
{
	SpawnProjectile(StartDirection, EndDirection);
	Multi_Fire();
}

void AWeaponBase::SpawnProjectile(FVector& StartDirection, FVector& EndDirection)
{
	if (!SkeletalMeshComponent) return;
	FVector MuzzleLocation = SkeletalMeshComponent->GetSocketLocation(FName("Muzzle"));
	
	CurrentAmmo--;
	
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
				float Damage = CalculateDamage(SphereHit.BoneName);
				IDamageInterface::Execute_TakeDamage(SphereHit.GetActor(), Damage, GetOwner());
			}
		}
	}
}

float AWeaponBase::CalculateDamage(FName BoneHit)
{
	float BaseDamage = WeaponData.Damage;
	float RandomDamage = FMath::FRandRange(BaseDamage * 0.7f, BaseDamage * 1.1f);
	float FinalDamage = RandomDamage;
	
	TMap<FName, float> BoneMultipliers = {
		{ "head", 2.5f },
		{ "neck1", 2.0f },
        
		{ "spine_01", 1.0f },
		{ "spine_02", 1.0f },
		{ "spine_03", 1.0f },
		{ "pelvis", 0.9f },

		{ "upperarm_l", 0.7f },
		{ "upperarm_r", 0.7f },
		{ "lowerarm_l", 0.7f },
		{ "lowerarm_r", 0.7f },

		{ "thigh_l", 0.6f },
		{ "thigh_r", 0.6f },
		{ "calf_l", 0.6f },
		{ "calf_r", 0.6f },
		{ "foot_l", 0.6f },
		{ "foot_r", 0.6f },
		{ "ball_l", 0.6f },
		{ "ball_r", 0.6f }
	};

	float BoneMultiplier = 1.0f;
	if (BoneMultipliers.Contains(BoneHit))
	{
		BoneMultiplier = BoneMultipliers[BoneHit];
	}

	FinalDamage *= BoneMultiplier;
	
	if (BoneHit == "head" || BoneHit == "neck1")
	{
		FinalDamage *= WeaponData.HeadMultiplier;
	}
	else if (
		BoneHit == "thigh_r" || BoneHit == "thigh_l" ||
		BoneHit == "calf_r" || BoneHit == "calf_l" ||
		BoneHit == "foot_r" || BoneHit == "foot_l" ||
		BoneHit == "ball_r" || BoneHit == "ball_l")
	{
		FinalDamage *= WeaponData.LegsMultiplier;
	}

	UE_LOG(LogTemp, Warning, TEXT("Bone: %s"), *BoneHit.ToString());
	UE_LOG(LogTemp, Warning, TEXT("Damage: %f"), FinalDamage);
	return FinalDamage;
}

void AWeaponBase::Multi_Fire_Implementation()
{
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
		if (LoadedFireAnimation)
		{
			SkeletalMeshComponent->PlayAnimation(LoadedFireAnimation, false);
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
	UAnimInstance* CharacterAnimInstance = Cast<ACharacter>(GetOwner())->GetMesh()->GetAnimInstance();
    if (IsValid(LoadedReloadCharacterMontage) && CharacterAnimInstance)
    {
    	CharacterAnimInstance->Montage_Play(LoadedReloadCharacterMontage);
    		
    	float Duration = LoadedReloadCharacterMontage->GetPlayLength();
    	FTimerHandle TimerHandle;
    	GetWorld()->GetTimerManager().SetTimer(
    		TimerHandle,
    		this,
    		&AWeaponBase::ReloadAfterDelay,
    		Duration <= 0.0f ? 1.0f : Duration,
    		false
    	);
    }

	
    const FSoftObjectPath MontagePath = WeaponData.ReloadCharacterMontage.ToSoftObjectPath();
    if (!MontagePath.IsValid()) return;
	
    UAssetManager::GetStreamableManager().RequestAsyncLoad(
        MontagePath,
        [this, CharacterAnimInstance]()
        {
            if (WeaponData.ReloadCharacterMontage.IsValid() && CharacterAnimInstance)
            {
                LoadedReloadCharacterMontage = WeaponData.ReloadCharacterMontage.Get();
            	CharacterAnimInstance->Montage_Play(LoadedReloadCharacterMontage);
                    	
            	float Duration = LoadedReloadCharacterMontage->GetPlayLength();
            	FTimerHandle TimerHandle;
            	GetWorld()->GetTimerManager().SetTimer(
            		TimerHandle,
            		this,
            		&AWeaponBase::ReloadAfterDelay,
            		Duration <= 0.0f ? 1.0f : Duration,
            		false
            	);
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
	/*USoundBase* HitSound = nullptr;
	UParticleSystem* HitEmitter = nullptr;
	UMaterialInterface* HitDecal = nullptr;

	switch (SurfaceType)
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
