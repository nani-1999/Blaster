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

	virtual void Tick(float DeltaTime) override;

	/* Fire */
	virtual void Fire(const FVector& ProjectileHitTarget) override;

protected:
	virtual void BeginPlay() override;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<AProjectile> ProjectileClass;
};
