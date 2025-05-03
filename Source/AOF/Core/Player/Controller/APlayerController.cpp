// Fill out your copyright notice in the Description page of Project Settings.


#include "APlayerController.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "AOF/Core/Inventory/Component/Inventory/InventoryComponent.h"
#include "AOF/Core/Inventory/Interface/ToItemInterface.h"
#include "AOF/Core/Player/Interfaces/InputPlayer/InputPlayerInterface.h"
#include "AOF/Core/Player/Interfaces/ToPlayer/ToPlayerInterface.h"
#include "AOF/Core/Weapon/Interface/ToWeapon/ToWeaponInterface.h"
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
		InputComponent->BindKey(EKeys::One, IE_Pressed, this, &AAPlayerController::SelectSlot);
		InputComponent->BindKey(EKeys::Two, IE_Pressed, this, &AAPlayerController::SelectSlot);
		InputComponent->BindKey(EKeys::Three, IE_Pressed, this, &AAPlayerController::SelectSlot);
		InputComponent->BindKey(EKeys::Four, IE_Pressed, this, &AAPlayerController::SelectSlot);
		InputComponent->BindKey(EKeys::Five, IE_Pressed, this, &AAPlayerController::SelectSlot);
		InputComponent->BindKey(EKeys::Six, IE_Pressed, this, &AAPlayerController::SelectSlot);
		
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
		if (CrouchAction)
		{
			EnhancedInput->BindAction(CrouchAction, ETriggerEvent::Triggered, this, &AAPlayerController::StartCrouch);
			EnhancedInput->BindAction(CrouchAction, ETriggerEvent::Completed, this, &AAPlayerController::StopCrouch);
		}
		if (UseItemAction)
		{
			EnhancedInput->BindAction(UseItemAction, ETriggerEvent::Started, this, &AAPlayerController::UseItem);
			EnhancedInput->BindAction(UseItemAction, ETriggerEvent::Completed, this, &AAPlayerController::StopUseItem);
		}
		if (ReloadAction)
		{
			EnhancedInput->BindAction(ReloadAction, ETriggerEvent::Started, this, &AAPlayerController::Reload);
		}
		if (InventoryAction)
		{
			EnhancedInput->BindAction(InventoryAction, ETriggerEvent::Started, this, &AAPlayerController::Inventory);
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
	FVector2D LookAxis = Value.Get<FVector2D>();
	
	AddYawInput(LookAxis.X);
	AddPitchInput(LookAxis.Y);		
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

void AAPlayerController::StartCrouch()
{
}

void AAPlayerController::StopCrouch()
{
}

void AAPlayerController::UseItem()
{
	if (ControlledCharacter)
	{
		if (auto ItemInHand = IToPlayerInterface::Execute_GetItemInHand(ControlledCharacter))
		{
			IToItemInterface::Execute_UseItem(ItemInHand);
		}
	}
}

void AAPlayerController::StopUseItem()
{
	if (ControlledCharacter)
	{
		if (auto ItemInHand = IToPlayerInterface::Execute_GetItemInHand(ControlledCharacter))
		{
			IToItemInterface::Execute_StopUseItem(ItemInHand);
		}
	}
}

void AAPlayerController::Reload()
{
	if (ControlledCharacter)
	{
		auto ItemInHand = IToPlayerInterface::Execute_GetItemInHand(ControlledCharacter);
		if (ItemInHand && ItemInHand->Implements<UToWeaponInterface>())
		{
			IToWeaponInterface::Execute_Reload(ItemInHand);
		}
	}
}

void AAPlayerController::Inventory()
{

}

void AAPlayerController::SelectSlot(FKey PressedKey)
{
	int32 KeyIndex = FCString::Atoi(*PressedKey.GetDisplayName().ToString());
	if (ControlledCharacter && KeyIndex <= 5)
	{
		if (UInventoryComponent* Inventory = ControlledCharacter->FindComponentByClass<UInventoryComponent>())
		{
			Inventory->Server_SpawnItemInHand(KeyIndex);
		}
	}		
}
