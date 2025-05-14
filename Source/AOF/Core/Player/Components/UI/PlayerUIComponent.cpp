// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerUIComponent.h"

#include "Blueprint/UserWidget.h"
#include "GameFramework/Character.h"


void UPlayerUIComponent::BeginPlay()
{
	Super::BeginPlay();
	
	ACharacter* Character = Cast<ACharacter>(GetOwner());
	if (Character)
	{
		PlayerController = Cast<APlayerController>(Character->GetController());
	}
}

UUserWidget* UPlayerUIComponent::CreatePlayerWidget(TSubclassOf<UUserWidget> Widget)
{
	if (Widget && PlayerController)
	{
		UUserWidget* PlayerWidget = CreateWidget(PlayerController, Widget);
		PlayerWidgets.Add(PlayerWidget);
		
		PlayerWidget->AddToViewport();
		return PlayerWidget;
	}
	return nullptr;
}

UUserWidget* UPlayerUIComponent::GetPlayerWidget(TSubclassOf<UUserWidget> Widget)
{
	for (UUserWidget* PlayerWidget : PlayerWidgets)
	{
		if (PlayerWidget->GetClass() == Widget)
		{
			return PlayerWidget;
		}
	}
	return nullptr;
}

void UPlayerUIComponent::RemovePlayerWidget(TSubclassOf<UUserWidget> Widget)
{
	for (UUserWidget* PlayerWidget : PlayerWidgets)
	{
		if (PlayerWidget->GetClass() == Widget)
		{
			PlayerWidget->RemoveFromParent();
			PlayerWidgets.Remove(PlayerWidget);
			break;
		}
	}
}
