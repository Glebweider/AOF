// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AOF/Core/Inventory/Component/Inventory/InventoryComponent.h"
#include "AOF/UI/Interface/ToUIInterface.h"
#include "AOF/UI/Widgets/InventorySlot/InventorySlotWidget.h"
#include "Blueprint/UserWidget.h"
#include "Components/HorizontalBox.h"
#include "Components/UniformGridPanel.h"
#include "PlayerHUD.generated.h"

/**
 * 
 */
UCLASS()
class AOF_API UPlayerHUD : public UUserWidget, public IToUIInterface
{
	GENERATED_BODY()

public:
	virtual void SetVisibilityInventory_Implementation(bool bIsInventoryOpen) override;
	void SetRotationCompassBar();

protected:
	virtual void NativeConstruct() override;
	
	virtual void FPS_Counter();
	virtual void CreateInventorySlots();
	virtual void BindToInventory(UInventoryComponent* InventoryComponent);
	
	UFUNCTION()
	virtual void UpdateInventory();
	
	UPROPERTY()
	UInventoryComponent* InventoryRef;

	UPROPERTY()
	TArray<UInventorySlotWidget*> InventorySlots;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="UMG HUD")
	TSubclassOf<UInventorySlotWidget> SlotWidgetClass;
	
	UPROPERTY(BlueprintReadWrite, Category="UMG HUD")
	FTimerHandle FPSTimerHandle;

	UPROPERTY(meta = (BindWidget))
	UUniformGridPanel* GridPanel_Inventory;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* FPSCounterText;

	UPROPERTY(meta = (BindWidget))
	UHorizontalBox* HorBox_Inventory;

	UPROPERTY(meta = (BindWidgetAnim), Transient)
	UWidgetAnimation* InventoryToggleAnimation;

	UPROPERTY(meta = (BindWidget))
	UImage* Img_CompassBar;
};
