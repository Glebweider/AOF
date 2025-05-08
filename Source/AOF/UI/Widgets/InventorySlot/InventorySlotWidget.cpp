// Fill out your copyright notice in the Description page of Project Settings.


#include "InventorySlotWidget.h"

#include "AOF/Core/Inventory/Structures/ItemStructure.h"
#include "AOF/Core/Weapon/WeaponBase.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"
#include "Components/Image.h"
#include "Components/Border.h"
#include "Components/CanvasPanel.h"


void UInventorySlotWidget::NativeConstruct()
{
	Super::NativeConstruct();
	ClearItem();
}

void UInventorySlotWidget::SetItem(const FInventoryItem& Item)
{
	if (Canvas)
	{
		Canvas->SetVisibility(ESlateVisibility::Visible);
		
		if (Icon_Image)
		{
			Icon_Image->SetBrushFromTexture(Item.Icon);
		}
		
		if (Quantity_Text)
		{
			Quantity_Text->SetVisibility(ESlateVisibility::Visible);
			
			/*if (Item.Quantity != 0)
			{
				Quantity_Text->SetText(FText::AsNumber(Item.Quantity));
			} else if (Item.ItemClass->IsChildOf(AWeaponBase::StaticClass()))
			{
				Quantity_Text->SetText(FText::AsNumber(Item.Quantity));
			} else
			{
				Quantity_Text->SetVisibility(ESlateVisibility::Collapsed);
			}*/

			if (Item.Quantity != 0)
			{
				Quantity_Text->SetText(FText::AsNumber(Item.Quantity));
			} else
			{
				Quantity_Text->SetVisibility(ESlateVisibility::Collapsed);
			}
		}

		if (Durability_ProgBar)
		{
			Durability_ProgBar->SetPercent(Item.MaxDurability > 0.0f ? Item.Durability / Item.MaxDurability : 0.0f);
		}
	}
}

void UInventorySlotWidget::ClearItem()
{
	if (Canvas)
	{
		Canvas->SetVisibility(ESlateVisibility::Collapsed);
	}
}