// Copyright Na9i Studio.

#pragma once

#include "CoreMinimal.h"
#include "Weapon/Weapon.h"
#include "HitScanWeapon.generated.h"

class UParticleSystem;

UCLASS()
class BLASTER_API AHitScanWeapon : public AWeapon
{
	GENERATED_BODY()
	
public:
	AHitScanWeapon();

	virtual void FireBullet(const FVector& HitTarget) override;

protected:
	/* Stats */
	UPROPERTY(EditDefaultsOnly, Category = "UserClass | HitScanWeapon")
	float Damage;

	/* Particle */
	UPROPERTY(EditDefaultsOnly, Category = "UserClass | HitScanWeapon")
	TObjectPtr<UParticleSystem> ImpactParticle;

	/* Sound */
	UPROPERTY(EditDefaultsOnly, Category = "UserClass | HitScanWeapon")
	TObjectPtr<USoundCue> ImpactSound;

	/* Multicast */
	UFUNCTION(NetMulticast, Unreliable)
	void MulticastImpact(FVector_NetQuantize ImpactPoint, FVector_NetQuantizeNormal ImpactNormal);
};
