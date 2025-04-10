// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "DamageInterface.generated.h"

UINTERFACE()
class UDamageInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class AOF_API IDamageInterface
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category="Interfaces")
	void TakeDamage(float Damage, AActor* Character);
};