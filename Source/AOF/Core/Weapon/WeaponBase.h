// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AOF/Core/Inventory/ItemBase.h"
#include "AOF/Core/Inventory/Interface/ToItemInterface.h"
#include "GameFramework/Actor.h"
#include "Structures/WeaponStructure.h"
#include "WeaponBase.generated.h"

UCLASS()
class AOF_API AWeaponBase : public AItemBase, public IToItemInterface
{
	GENERATED_BODY()

public:
	AWeaponBase();

	/** Construction & Lifecycle */
	virtual void OnConstruction(const FTransform& Transform) override;
	virtual void BeginPlay() override;

	/** Interface */
	virtual void InteractItem_Implementation(AActor* CharacterInteract) override;
	virtual void UseItem_Implementation() override;
	virtual void StopUseItem_Implementation() override;

	/** Server RPCs */
	UFUNCTION(Server, Reliable)
	void Server_Fire(FVector StartDirection, FVector EndDirection);

	UFUNCTION(Server, Reliable)
	void Server_Reload();

	/** Multicast RPCs */
	UFUNCTION(NetMulticast, Reliable)
	void Multi_Reload();

	UFUNCTION(NetMulticast, Reliable)
	void Multi_Fire();

	UFUNCTION(NetMulticast, Reliable)
	void Multi_Fire_Recoil();

	UFUNCTION(NetMulticast, Reliable)
	void Multi_Spawn_Hit(EPhysicalSurface SurfaceType, FVector Location, UPrimitiveComponent* Component);

protected:
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	
	/** Weapon Usage */
	virtual void Fire();
	virtual void AutoFire();
	virtual void BurstFire();
	virtual void AutoReload();
	virtual bool CanFire();

	/** Helpers */
	void SpawnProjectile(FVector& StartDirection, FVector& EndDirection);
	void ReloadAfterDelay();
	void CalculateAmmo();

	/** Fire Logic */
	FTimerHandle FireTimerHandle;
	int32 CurrentBulletsInBurst = 0;

	UPROPERTY(Replicated)
	int32 CurrentAmmo = 0;

	UPROPERTY(Replicated)
	bool bIsReloading = false;

	UPROPERTY()
	bool bIsFire = false;

	UPROPERTY()
	bool bIsShooting = false;

	/** Assets */
	UPROPERTY()
	USoundCue* LoadedMuzzleSound = nullptr;

	UPROPERTY()
	UParticleSystem* LoadedMuzzleEmitter = nullptr;

	UPROPERTY()
	UAnimMontage* LoadedReloadCharacterMontage = nullptr;

	/** References */
	UPROPERTY()
	USkeletalMeshComponent* SkeletalMeshComponent = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon")
	AActor* Character = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon")
	AController* Controller = nullptr;

	/** Data */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon")
	FName RowName;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
	FDataTableRowHandle DataTableWeapon;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon")
	FWeaponData WeaponData;
};