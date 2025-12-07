// Copyright Na9i Studio.

#pragma once

#include "CoreMinimal.h"
#include "Weapon/HitScanWeapon.h"
#include "MultiHitScanWeapon.generated.h"

/**
 * 
 */
UCLASS()
class BLASTER_API AMultiHitScanWeapon : public AHitScanWeapon
{
	GENERATED_BODY()
	
public:
	AMultiHitScanWeapon();

	virtual void FireBullet(const FVector& HitTarget) override;
};
