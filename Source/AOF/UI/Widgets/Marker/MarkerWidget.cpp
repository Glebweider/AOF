// Fill out your copyright notice in the Description page of Project Settings.


#include "MarkerWidget.h"

void UMarkerWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (WidgetSwitcher)
	{
		switch (MarkerType)
		{
			case ECompassMarkerType::Enemy:
				WidgetSwitcher->SetActiveWidgetIndex(0);
			case ECompassMarkerType::Quest:
				WidgetSwitcher->SetActiveWidgetIndex(1);
			case ECompassMarkerType::Ally:
				WidgetSwitcher->SetActiveWidgetIndex(1);
		}
	}
}
