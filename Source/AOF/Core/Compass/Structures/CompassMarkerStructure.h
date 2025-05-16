#pragma once

#include "CoreMinimal.h"
#include "CompassMarkerStructure.generated.h"

class UMarkerWidget;

UENUM(BlueprintType)
enum class ECompassMarkerType : uint8
{
	Enemy UMETA(DisplayName = "Enemy"),
	Ally UMETA(DisplayName = "Ally"),
	Quest UMETA(DisplayName = "Quest"),
};


USTRUCT(BlueprintType)
struct AOF_API FCompassMarkerData
{
	GENERATED_BODY()

	UPROPERTY()
	AActor* Actor = nullptr;

	UPROPERTY()
	UMarkerWidget* Widget = nullptr;
};