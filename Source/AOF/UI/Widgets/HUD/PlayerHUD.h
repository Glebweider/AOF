// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AOF/Core/Compass/Structures/CompassMarkerStructure.h"
#include "AOF/Core/Inventory/Component/Inventory/InventoryComponent.h"
#include "AOF/UI/Interface/ToUIInterface.h"
#include "AOF/UI/Widgets/InventorySlot/InventorySlotWidget.h"
#include "AOF/UI/Widgets/Marker/MarkerWidget.h"
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
	virtual void AddMarkerToPlayerCompass_Implementation(ECompassMarkerType MarkerType, AActor* Actor) override;
	virtual void SetVisibilityInventory_Implementation(bool bIsInventoryOpen) override;

protected:
	virtual void NativeConstruct() override;
	
	virtual void FPS_Counter();
	virtual void CreateInventorySlots();
	virtual void SetRotationCompassBar();
	virtual void SetRotationMarkersCompassBar();
	virtual void BindToInventory(UInventoryComponent* InventoryComponent);
	
	UFUNCTION()
	virtual void UpdateInventory();
	
	UPROPERTY()
	TArray<FCompassMarkerData> CompassMarkers;

	UPROPERTY()
	ACharacter* PlayerCharacter;
	
	UPROPERTY()
	UInventoryComponent* InventoryRef;

	UPROPERTY()
	TArray<UInventorySlotWidget*> InventorySlots;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="UMG HUD")
	TSubclassOf<UInventorySlotWidget> SlotWidgetClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="UMG HUD")
	TSubclassOf<UMarkerWidget> MarkerWidgetClass;
	
	UPROPERTY()
	FTimerHandle FPSTimerHandle;
	
	UPROPERTY()
	FTimerHandle RotationCompassBarTimerHandle;

	UPROPERTY()
	FTimerHandle RotationMarkersCompassBarTimerHandle;

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
	
	UPROPERTY(meta = (BindWidget))
	UCanvasPanel* Canvas_CompassBar;
};
