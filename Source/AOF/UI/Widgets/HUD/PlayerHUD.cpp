// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerHUD.h"

#include "AOF/UI/Widgets/Marker/MarkerWidget.h"
#include "Blueprint/WidgetLayoutLibrary.h"
#include "Components/CanvasPanel.h"
#include "Components/CanvasPanelSlot.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "GameFramework/Character.h"
#include "Kismet/KismetMathLibrary.h"


void UPlayerHUD::NativeConstruct()
{
	Super::NativeConstruct();

	PlayerCharacter = Cast<ACharacter>(GetOwningPlayerPawn());

	FPS_Counter();
	GetWorld()->GetTimerManager().SetTimer(
		FPSTimerHandle,
		this,
		&UPlayerHUD::FPS_Counter,
		1.0f,
		true);

	if (ACharacter* Character = Cast<ACharacter>(GetOwningPlayerPawn()))
	{
		if (auto InventoryComp = Character->FindComponentByClass<UInventoryComponent>())
		{
			CreateInventorySlots();
			BindToInventory(InventoryComp);
		}

		GetWorld()->GetTimerManager().SetTimer(
			RotationCompassBarTimerHandle,
			this,
			&UPlayerHUD::SetRotationCompassBar,
			0.01f,
			true);

		GetWorld()->GetTimerManager().SetTimer(
			RotationMarkersCompassBarTimerHandle,
			this,
			&UPlayerHUD::SetRotationMarkersCompassBar,
			0.01f,
			true);
	}
}

void UPlayerHUD::FPS_Counter()
{
	int8 FPS = UKismetMathLibrary::FTrunc(1 / GetWorld()->GetDeltaSeconds());
	if (FPSCounterText)
	{
		FString FPSText = FString::FromInt(FPS);
		FPSCounterText->SetText(FText::FromString(FPSText));
	}
}

void UPlayerHUD::CreateInventorySlots()
{
	if (!SlotWidgetClass || !HorBox_Inventory || !GridPanel_Inventory) return;

	InventorySlots.Empty();
	HorBox_Inventory->ClearChildren();
	GridPanel_Inventory->ClearChildren();
	
	for (int32 i = 0; i < 6; ++i)
	{
		auto SlotWidget = CreateWidget<UInventorySlotWidget>(this, SlotWidgetClass);
		InventorySlots.Add(SlotWidget);
		HorBox_Inventory->AddChild(SlotWidget);
	}
	
	const int32 TotalSlots = 24;
	const int32 Columns = 6;
	for (int32 i = 6; i < TotalSlots; ++i)
	{
		auto SlotWidget = CreateWidget<UInventorySlotWidget>(this, SlotWidgetClass);
		InventorySlots.Add(SlotWidget);

		int32 Row = (i - 6) / Columns;
		int32 Col = (i - 6) % Columns;

		GridPanel_Inventory->AddChildToUniformGrid(SlotWidget, Row, Col);
	}
}

void UPlayerHUD::UpdateInventory()
{
	if (HorBox_Inventory)
	{
		auto Items = InventoryRef->GetItems();
		for (int32 i = 0; i < InventorySlots.Num(); ++i)
		{
			if (InventorySlots[i])
			{
				if (Items.IsValidIndex(i))
				{
					InventorySlots[i]->SetItem(Items[i]);
				}
				else
				{
					InventorySlots[i]->ClearItem();
				}
			}
		}
	}
}

void UPlayerHUD::BindToInventory(UInventoryComponent* InventoryComponent)
{
	if (!InventoryComponent) return;

	InventoryRef = InventoryComponent;
	InventoryRef->OnInventoryChanged.AddDynamic(this, &UPlayerHUD::UpdateInventory);
}

void UPlayerHUD::SetVisibilityInventory_Implementation(bool bIsInventoryOpen)
{
	if (InventoryToggleAnimation)
	{
		if (bIsInventoryOpen)
		{
			PlayAnimationForward(InventoryToggleAnimation);
		} else
		{
			PlayAnimationReverse(InventoryToggleAnimation);
		}
	}
}

void UPlayerHUD::SetRotationCompassBar()
{
	if (Img_CompassBar)
	{
		if (PlayerCharacter)
		{
			float Yaw = PlayerCharacter->GetActorRotation().Yaw;

			UCanvasPanelSlot* CanvasSlot = UWidgetLayoutLibrary::SlotAsCanvasSlot(Img_CompassBar);
			if (CanvasSlot)
			{
				float RotationX = (Yaw * -1.0f) * 10.0f;
				CanvasSlot->SetPosition(FVector2D(RotationX, 0.f));
			}
		}
	}
}

void UPlayerHUD::AddMarkerToPlayerCompass_Implementation(ECompassMarkerType MarkerType, AActor* Actor)
{
	if (Canvas_CompassBar && Actor)
	{
		if (auto Widget = CreateWidget<UMarkerWidget>(this, MarkerWidgetClass))
		{
			Widget->MarkerType = MarkerType;

			if (UCanvasPanelSlot* CanvasSlot = Canvas_CompassBar->AddChildToCanvas(Widget))
			{
				CanvasSlot->SetSize(FVector2D(32.0f, 32.0f));
				CanvasSlot->SetPosition(FVector2D(0.f, 0.f));			
			}

			FCompassMarkerData MarkerData;
			MarkerData.Actor = Actor;
			MarkerData.Widget = Widget;

			CompassMarkers.Add(MarkerData);
		}
	}
}

void UPlayerHUD::SetRotationMarkersCompassBar()
{
	if (!PlayerCharacter) return;
	
	for (const FCompassMarkerData& MarkerData : CompassMarkers)
	{
		AActor* TargetActor = MarkerData.Actor;
		UMarkerWidget* MarkerWidget = MarkerData.Widget;

		if (!TargetActor || !MarkerWidget) return;

		FVector CharacterLocation = PlayerCharacter->GetActorLocation();
		FVector TargetLocation = TargetActor->GetActorLocation();

		FRotator LookAtRotation = (TargetLocation - CharacterLocation).Rotation();
		float TargetYaw = LookAtRotation.Yaw;
		float CharacterYaw =  PlayerCharacter->GetActorRotation().Yaw;

		float DeltaYaw = FMath::FindDeltaAngleDegrees(CharacterYaw, TargetYaw);
		float CompassX = DeltaYaw * 10.0f;

		if (UCanvasPanelSlot* CanvasSlot = Cast<UCanvasPanelSlot>(MarkerWidget->Slot))
		{
			CanvasSlot->SetPosition(FVector2D(CompassX, 0.f));
		}
	}
}