#pragma once

#include "CoreMinimal.h"

UENUM()
enum class ECombatType : uint8 {
	EWT_Reloading,
	EWT_UnOccupied,
	EWT_MAX
};