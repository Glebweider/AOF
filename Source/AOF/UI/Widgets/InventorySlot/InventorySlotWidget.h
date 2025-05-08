// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "InventorySlotWidget.generated.h"

class UCanvasPanel;
class UBorder;
class UImage;
class UProgressBar;
class UTextBlock;
struct FInventoryItem;
/**
 * 
 */
UCLASS()
class AOF_API UInventorySlotWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable)
	void SetItem(const FInventoryItem& Item);
	
	UFUNCTION(BlueprintCallable)
	void ClearItem();

protected:
	virtual void NativeConstruct() override;

	UPROPERTY(meta = (BindWidget))
	UCanvasPanel* Canvas;
	
	UPROPERTY(meta = (BindWidget))
	UTextBlock* Quantity_Text;
	
	UPROPERTY(meta = (BindWidget))
	UProgressBar* Durability_ProgBar;
	
	UPROPERTY(meta = (BindWidget))
	UImage* Icon_Image;
	
	UPROPERTY(meta = (BindWidget))
	UBorder* Slot_Border;
};
