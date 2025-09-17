#pragma once

#include "CoreMinimal.h"

/* UENUM() is in CoreMinimal.h 
 * need UENUM() if we make variable of the enum
 * we don't need UENUM() if we just using the enum */

UENUM() 
enum class EWeaponType : uint8 {
	EWT_AssaultRifle,
	EWT_MAX /* max is actually used to get the size of enum */
};

namespace EWeaponTypeStr
{
	/* enums doesn't support namespaces if we are creating class variables */
	inline const TCHAR* ToString(EWeaponType WeaponType)
	{
		switch (WeaponType) {
			case EWeaponType::EWT_AssaultRifle: return TEXT("AssaultRifle");
			default: return TEXT("MAX");
		};
	}
}