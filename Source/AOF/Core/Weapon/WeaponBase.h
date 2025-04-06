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

protected:
	UPROPERTY()
	USkeletalMeshComponent* SkeletalMeshComponent;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FDataTableRowHandle DataTableWeapon;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WeaponData")
	FWeaponData WeaponData;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WeaponData")
	int32 CurrentAmmo;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon")
	AActor* Character;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon")
	AController* Controller;
	
};