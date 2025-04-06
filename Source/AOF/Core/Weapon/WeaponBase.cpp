// Fill out your copyright notice in the Description page of Project Settings.


#include "WeaponBase.h"

#include "Engine/AssetManager.h"
#include "Structures/WeaponStructure.h"

AWeaponBase::AWeaponBase()
{
	PrimaryActorTick.bCanEverTick = false;
}


void AWeaponBase::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);

	
	if (DataTableWeapon.IsNull()) return;

	if (const auto WeaponDataRow = DataTableWeapon.GetRow<FWeaponStruct>(""))
	{
		WeaponData.Damage = WeaponDataRow->Damage;
		WeaponData.FireSpeed = WeaponDataRow->FireSpeed / 60;
		WeaponData.BulletInShoot = WeaponDataRow->BulletInShoot;
		WeaponData.AmmoMagazine = WeaponDataRow->AmmoMagazine;
		WeaponData.MaxAmmoMagazine = WeaponDataRow->MaxAmmoMagazine;
		
		WeaponData.HeadMultiplier = WeaponDataRow->HeadMultiplier;
		WeaponData.LegsMultiplier = WeaponDataRow->LegsMultiplier;
		WeaponData.RecoilMultiplier = WeaponDataRow->RecoilMultiplier;
		
		WeaponData.FireType = WeaponDataRow->FireType;
		WeaponData.AmmoType = WeaponDataRow->AmmoType;

		WeaponData.HandSocket = WeaponDataRow->HandSocket;
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
			FSoftObjectPath WeaponMeshPath = WeaponData.WeaponMesh.ToSoftObjectPath();
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
	
}