#pragma once

#include "CoreMinimal.h"

UENUM()
enum class ECombatState : uint8 {
	ECS_Reloading,
	ECS_UnOccupied,
	ECS_MAX
};

namespace ECombatStateStr
{
	/* enums doesn't support namespaces if we are creating class variables */
	inline const TCHAR* ToString(ECombatState CombatState)
	{
		switch (CombatState) {
			case ECombatState::ECS_Reloading: return TEXT("Reloading");
			case ECombatState::ECS_UnOccupied: return TEXT("UnOccupied");
			default: return TEXT("MAX");
		};
	}
}