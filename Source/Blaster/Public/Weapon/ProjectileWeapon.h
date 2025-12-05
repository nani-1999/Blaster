// Copyright Na9i Studio.

#pragma once

#include "CoreMinimal.h"
#include "Weapon/Weapon.h"
#include "ProjectileWeapon.generated.h"

class AProjectile;

UCLASS()
class BLASTER_API AProjectileWeapon : public AWeapon
{
	GENERATED_BODY()
	
public:
	AProjectileWeapon();

	/* Fire Bullet */
	virtual void FireBullet(const FVector& HitTarget) override;

protected:
	UPROPERTY(EditDefaultsOnly, Category = "UserClass | ProjectileWeapon")
	TSubclassOf<AProjectile> ProjectileClass;
};
