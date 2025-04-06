// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "InputMappingContext.h"
#include "APlayerController.generated.h"

/**
 * 
 */
UCLASS()
class AOF_API AAPlayerController : public APlayerController
{
	GENERATED_BODY()
	
public:
	AAPlayerController();

protected:
	virtual void BeginPlay() override;
	virtual void SetupInputComponent() override;
	
	void MoveForward(float Value);
	void MoveRight(float Value);
	void Turn(float Value);
	void LookUp(float Value);
	void StartJump();
	void StopJump();
	
private:
	UPROPERTY(EditDefaultsOnly, Category = "Input")
	UInputMappingContext* InputMappingContext;
	
	//UEnhancedInputSubsystemLocal* EnhancedInputSubsystem;
};
