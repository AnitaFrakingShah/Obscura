#pragma once

#include "UObject/ObjectMacros.h"

UENUM(BlueprintType)
enum ECustomMovementMode
{
	CLIMBING      UMETA(DisplayName = "Climbing"),
	MAX			  UMETA(Hidden),
};