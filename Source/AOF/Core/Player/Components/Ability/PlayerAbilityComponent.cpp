// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerAbilityComponent.h"


UPlayerAbilityComponent::UPlayerAbilityComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}


void UPlayerAbilityComponent::BeginPlay()
{
	Super::BeginPlay();

	Health = MaxHealth;
}


