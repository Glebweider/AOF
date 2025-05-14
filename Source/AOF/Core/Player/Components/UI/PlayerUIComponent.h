// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "PlayerUIComponent.generated.h"


UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class AOF_API UPlayerUIComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	bool bIsUIActive = false;
	
	UFUNCTION()
	UUserWidget* CreatePlayerWidget(TSubclassOf<UUserWidget> Widget);

	UFUNCTION()
	UUserWidget* GetPlayerWidget(TSubclassOf<UUserWidget> Widget);
	
	UFUNCTION()
	void RemovePlayerWidget(TSubclassOf<UUserWidget> Widget);

protected:
	virtual void BeginPlay() override;

	UPROPERTY()
	APlayerController* PlayerController;

	UPROPERTY()
	TArray<UUserWidget*> PlayerWidgets;
};
