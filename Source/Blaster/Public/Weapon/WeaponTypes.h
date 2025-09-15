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

//namespace EWeaponType
//{
//	enum Type
//	{
//		AssaultRifle,
//		MAX
//	};
//
//	inline const TCHAR* ToString(EWeaponType::Type WeaponType)
//	{
//		switch (WeaponType)
//		{
//			case AssaultRifle: return TEXT("AssaultRifle");
//			case MAX: return TEXT("MAX");
//		};
//	}
//}