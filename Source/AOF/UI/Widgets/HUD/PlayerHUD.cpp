// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerHUD.h"

#include "Components/TextBlock.h"
#include "GameFramework/Character.h"
#include "Kismet/KismetMathLibrary.h"
#include "Net/UnrealNetwork.h"

void UPlayerHUD::NativeConstruct()
{
	Super::NativeConstruct();

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
	UE_LOG(LogTemp, Display, TEXT("UPlayerHUD::UpdateInventory"));
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
