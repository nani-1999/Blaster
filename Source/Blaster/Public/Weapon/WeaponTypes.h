#pragma once

#include "CoreMinimal.h"

/* UENUM() is in CoreMinimal.h 
 * need UENUM() if we make variable of the enum
 * we don't need UENUM() if we just using the enum */

UENUM() 
enum class EWeaponType : uint8 {
	EWT_AssaultRifle,
	EWT_MAX
};