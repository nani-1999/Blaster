#pragma once

#include "CoreMinimal.h"

/* UENUM() is in CoreMinimal.h 
 * need UENUM() if we make variable of the enum
 * we don't need UENUM() if we just using the enum */

UENUM() 
enum class EWeaponType : uint8 {
	EWT_AssaultRifle,
	EWT_RocketLauncher,
	EWT_Pistol,
	EWT_SMG,
	EWT_Shotgun,
	EWT_MAX /* max is actually used to get the size of enum */
};

namespace EWeaponTypeStr
{
	/* enums doesn't support namespaces if we are creating class variables */
	inline const TCHAR* ToString(EWeaponType WeaponType)
	{
		switch (WeaponType) {
			case EWeaponType::EWT_AssaultRifle: return TEXT("AssaultRifle");
			case EWeaponType::EWT_RocketLauncher: return TEXT("RocketLauncher");
			case EWeaponType::EWT_Pistol: return TEXT("Pistol");
			case EWeaponType::EWT_SMG: return TEXT("SMG");
			case EWeaponType::EWT_Shotgun: return TEXT("Shotgun");
			default: return TEXT("MAX");
		};
	}
}