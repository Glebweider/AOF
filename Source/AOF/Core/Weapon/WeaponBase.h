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
	
	virtual void OnConstruction(const FTransform& Transform) override;
	virtual void BeginPlay() override;
	
	virtual void InteractItem_Implementation(AActor* CharacterInteract) override;
	virtual void UseItem_Implementation() override;
	virtual void StopUseItem_Implementation() override;

	virtual void Fire();
	virtual void AutoFire();
	virtual void BurstFire();
	virtual void SpawnProjectile(FVector& StartDirection, FVector& EndDirection);
	
	virtual void AutoReload();
	
	virtual bool CanFire();

	UFUNCTION(Server, Reliable)
	void Server_Fire(FVector StartDirection, FVector EndDirection);

	UFUNCTION(NetMulticast, Reliable)
	void Multi_Fire();

	UFUNCTION(NetMulticast, Reliable)
	void Multi_Fire_Recoil();
	
	UFUNCTION(NetMulticast, Reliable)
	void Multi_Spawn_Hit();


protected:
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	
	FTimerHandle FireTimerHandle;
	
	int32 CurrentBulletsInBurst;
	
	UPROPERTY(Replicated)
	int32 CurrentAmmo;

	bool bIsFire = false;
	bool bIsShooting = false;
	
	UPROPERTY()
	USkeletalMeshComponent* SkeletalMeshComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon")
	FName RowName;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FDataTableRowHandle DataTableWeapon;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WeaponData")
	FWeaponData WeaponData;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon")
	AActor* Character;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon")
	AController* Controller;
	
};