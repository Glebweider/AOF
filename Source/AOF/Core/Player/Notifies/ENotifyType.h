#pragma once

#include "ENotifyType.generated.h"

UENUM(BlueprintType)
enum class ENotifyType : uint8
{
	Remove_Magazine UMETA(DisplayName = "Remove Magazine"),
	Drop_Magazine   UMETA(DisplayName = "Drop Magazine"),
	Take_Magazine   UMETA(DisplayName = "Take Magazine"),
	Place_Magazine  UMETA(DisplayName = "Place Magazine")
};
