// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Structures/CompassMarkerStructure.h"
#include "CompassMarkerComponent.generated.h"

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class AOF_API UCompassMarkerComponent : public UActorComponent
{
	GENERATED_BODY()


public:
	UCompassMarkerComponent();
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Data")
	ECompassMarkerType MarkerType = ECompassMarkerType::Quest;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Data")
	TSubclassOf<UUserWidget> PlayerHUDWidget;

	UFUNCTION()
	void InitCompassForLocalPlayer();

protected:
	virtual void BeginPlay() override;
};
