#pragma once

#include "CoreMinimal.h"

UENUM()
enum class ECombatState : uint8 {
	ECS_UnOccupied,
	ECS_Reloading,
	ECS_MAX
};

namespace ECombatStateStr
{
	/* enums doesn't support namespaces if we are creating class variables */
	inline const TCHAR* ToString(ECombatState CombatState)
	{
		switch (CombatState) {
			case ECombatState::ECS_UnOccupied: return TEXT("UnOccupied");
			case ECombatState::ECS_Reloading: return TEXT("Reloading");
			default: return TEXT("MAX");
		};
	}
}