// Fill out your copyright notice in the Description page of Project Settings.


#include "APlayerController.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "AOF/Core/Player/Interfaces/InputPlayer/InputPlayerInterface.h"
#include "GameFramework/Character.h"

void AAPlayerController::BeginPlay()
{
	Super::BeginPlay();
	
	ControlledCharacter = Cast<ACharacter>(GetPawn());
	if (ULocalPlayer* LocalPlayer = GetLocalPlayer())
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = LocalPlayer->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>())
		{
			if (InputMappingContext)
			{
				Subsystem->AddMappingContext(InputMappingContext, 0);
			}
		}
	}
}

void AAPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();
	
	if (UEnhancedInputComponent* EnhancedInput = Cast<UEnhancedInputComponent>(InputComponent))
	{
		if (MoveAction)
		{
			EnhancedInput->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AAPlayerController::Move);
		}
		if (JumpAction)
		{
			EnhancedInput->BindAction(JumpAction, ETriggerEvent::Started, this, &AAPlayerController::StartJump);
			EnhancedInput->BindAction(JumpAction, ETriggerEvent::Completed, this, &AAPlayerController::StopJump);
		}
		if (LookAction)
		{
			EnhancedInput->BindAction(LookAction, ETriggerEvent::Triggered, this, &AAPlayerController::Look);
		}
		if (SprintAction)
		{
			EnhancedInput->BindAction(SprintAction, ETriggerEvent::Triggered, this, &AAPlayerController::StartSprint);
			EnhancedInput->BindAction(SprintAction, ETriggerEvent::Completed, this, &AAPlayerController::StopSprint);
		}
		if (InteractAction)
		{
			EnhancedInput->BindAction(InteractAction, ETriggerEvent::Started, this, &AAPlayerController::Interact);
		}
	}
}

void AAPlayerController::Move(const FInputActionValue& Value)
{
	if (ControlledCharacter)
	{
		FVector2D MovementVector = Value.Get<FVector2D>();
		FRotator YawRotation(0, GetControlRotation().Yaw, 0);
		
		FVector ForwardDir = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		FVector RightDir = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

		ControlledCharacter->AddMovementInput(ForwardDir, MovementVector.Y);
		ControlledCharacter->AddMovementInput(RightDir, MovementVector.X);		
	}
}

void AAPlayerController::Look(const FInputActionValue& Value)
{
	if (ControlledCharacter)
	{
		FVector2D LookAxis = Value.Get<FVector2D>();
		
		AddYawInput(LookAxis.X);
		AddPitchInput(LookAxis.Y);		
	}
}


void AAPlayerController::StartJump()
{
	if (ControlledCharacter)
	{
		ControlledCharacter->Jump();
	}
}

void AAPlayerController::StopJump()
{
	if (ControlledCharacter)
	{
		ControlledCharacter->StopJumping();
	}
}

void AAPlayerController::StartSprint()
{
	if (ControlledCharacter)
	{
		IInputPlayerInterface::Execute_HandleSprint(ControlledCharacter, true);
	}
}

void AAPlayerController::StopSprint()
{
	if (ControlledCharacter)
	{
		IInputPlayerInterface::Execute_HandleSprint(ControlledCharacter, false);
	}
}

void AAPlayerController::Interact()
{
	if (ControlledCharacter)
	{
		IInputPlayerInterface::Execute_HandleInteract(ControlledCharacter);
	}
}