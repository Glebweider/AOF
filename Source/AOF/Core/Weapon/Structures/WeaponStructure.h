// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "WeaponStructure.generated.h"

/**
 * 
 */
UENUM(BlueprintType)
enum class EFireType : uint8
{
	Auto UMETA(DisplayName = "Auto"),
	Burst UMETA(DisplayName = "Burst"),
	Single UMETA(DisplayName = "Single"),
};

UENUM(BlueprintType)
enum class EAmmoType : uint8
{
	Rifle UMETA(DisplayName = "5x56 Rifle"),
	Pistol UMETA(DisplayName = "9x19 Pistol"),
	Sniper UMETA(DisplayName = "7x62 Sniper"),
};

USTRUCT(BlueprintType)
struct AOF_API FWeaponStruct : public FTableRowBase
{
	GENERATED_BODY()

	FWeaponStruct()
		: Damage(1)
		, BulletInShoot(1)
		, AmmoMagazine(1)
		, MaxAmmoMagazine(30)
		, HeadMultiplier(2)
		, LegsMultiplier(1)
		, RecoilMultiplier(1)
		, FireSpeed(200)
		, FireType(EFireType::Auto)
		, AmmoType(EAmmoType::Rifle)
	{}

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	int32 Damage;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	int32 BulletInShoot;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	int32 AmmoMagazine;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	int32 MaxAmmoMagazine;
	
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float HeadMultiplier;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float LegsMultiplier;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float RecoilMultiplier;
		
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float FireSpeed;
	
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	EFireType FireType;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	EAmmoType AmmoType;
	
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FName Name;
	
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TSoftObjectPtr<USkeletalMesh> WeaponMesh;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TSoftObjectPtr<UStaticMesh> MagazineMesh;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TSoftObjectPtr<USoundCue> MuzzleSound;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TSoftObjectPtr<UParticleSystem> MuzzleEmitter;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TSoftObjectPtr<UAnimSequence> FireAnimation;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TSoftObjectPtr<UAnimMontage> ReloadCharacterMontage;
};

USTRUCT(BlueprintType)
struct AOF_API FWeaponData
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	int32 Damage = 0;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	int32 BulletInShoot = 0;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	int32 AmmoMagazine = 0;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	int32 MaxAmmoMagazine = 0;
	
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float HeadMultiplier = 1.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float LegsMultiplier = 1.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float RecoilMultiplier = 1.0f;
		
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float FireSpeed = 1.0f;
	
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	EFireType FireType = EFireType::Auto;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	EAmmoType AmmoType = EAmmoType::Rifle;
	
		
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FName Name = FName("None");
	
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TSoftObjectPtr<USkeletalMesh> WeaponMesh = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TSoftObjectPtr<UStaticMesh> MagazineMesh = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TSoftObjectPtr<USoundCue> MuzzleSound = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TSoftObjectPtr<UParticleSystem> MuzzleEmitter = nullptr;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TSoftObjectPtr<UAnimSequence> FireAnimation;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TSoftObjectPtr<UAnimMontage> ReloadCharacterMontage = nullptr;
};