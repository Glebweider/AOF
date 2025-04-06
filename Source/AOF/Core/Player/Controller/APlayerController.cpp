// Fill out your copyright notice in the Description page of Project Settings.


#include "APlayerController.h"

#include "AOF/Core/Player/Character/APlayerCharacter.h"

AAPlayerController::AAPlayerController()
{
	
}

void AAPlayerController::BeginPlay()
{
	Super::BeginPlay();
	
	//PlayerCharacter = Cast<AAPlayerCharacter>(GetCharacter());
}

void AAPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();
	
	InputComponent->BindAxis("MoveForward", this, &AAPlayerController::MoveForward);
	InputComponent->BindAxis("MoveRight", this, &AAPlayerController::MoveRight);
	InputComponent->BindAxis("Turn", this, &AAPlayerController::Turn);
	InputComponent->BindAxis("LookUp", this, &AAPlayerController::LookUp);

	InputComponent->BindAction("Jump", IE_Pressed, this, &AAPlayerController::StartJump);
	InputComponent->BindAction("Jump", IE_Released, this, &AAPlayerController::StopJump);
}

void AAPlayerController::MoveForward(float Value)
{
	/*if (PlayerCharacter)
	{
		PlayerCharacter->AddMovementInput(PlayerCharacter->GetActorForwardVector(), Value);
	}*/
}

void AAPlayerController::MoveRight(float Value)
{
	/*if (PlayerCharacter)
	{
		PlayerCharacter->AddMovementInput(PlayerCharacter->GetActorRightVector(), Value);
	}*/
}

void AAPlayerController::StartJump()
{
	/*if (PlayerCharacter)
	{
		PlayerCharacter->Jump();
	}*/
}

void AAPlayerController::StopJump()
{
	/*if (PlayerCharacter)
	{
		PlayerCharacter->StopJumping();
	}*/
}

void AAPlayerController::Turn(float Value)
{
	/*if (PlayerCharacter)
	{
		PlayerCharacter->AddControllerYawInput(Value);
	}*/
}

void AAPlayerController::LookUp(float Value)
{
	/*if (PlayerCharacter)
	{
		PlayerCharacter->AddControllerPitchInput(Value);
	}*/
}