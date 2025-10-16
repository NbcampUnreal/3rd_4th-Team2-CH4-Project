// TTeamTypes.h

#pragma once
#include "TTeamTypes.Generated.h"

UENUM(BlueprintType)
enum class ETeam : uint8
{
	None UMETA(DisplayName="None"),
	Police UMETA(DisplayName="Police"),
	Thief UMETA(DisplayName="Thief"),
};