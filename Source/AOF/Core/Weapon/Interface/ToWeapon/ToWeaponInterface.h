// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "ToWeaponInterface.generated.h"


UINTERFACE()
class UToWeaponInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class AOF_API IToWeaponInterface
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category="Interfaces")
	bool GetIsReloading();
	
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category="Interfaces")
	UStaticMesh* GetMagazineMesh();
	
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category="Interfaces")
	UStaticMeshComponent* GetMagazineMeshComponent();

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category="Interfaces")
	USkeletalMeshComponent* GetWeaponSkeletalMeshComponent();

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category="Interfaces")
	void SetMagazineVariableSkeletalMeshComponent(UStaticMeshComponent* MagMesh);
};
