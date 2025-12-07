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

	/* Bullet */
	UPROPERTY(EditDefaultsOnly, Category = "UserClass | HitScanWeapon");
	uint8 PelletsPerBullet;

	/* Scatter */
	UPROPERTY(EditDefaultsOnly, Category = "UserClass | HitScanWeapon");
	bool bScatter;
	UPROPERTY(EditDefaultsOnly, Category = "UserClass | HitScanWeapon");
	float ScatterSize;

	/* Trace/HitScan */
	void TraceAndDamage(FVector StartPoint, FVector EndPoint, APawn* InstigatorPawn);

	/* Particle */
	UPROPERTY(EditDefaultsOnly, Category = "UserClass | HitScanWeapon")
	TObjectPtr<UParticleSystem> ImpactParticles;
	UPROPERTY(EditDefaultsOnly, Category = "UserClass | HitScanWeapon")
	TObjectPtr<UParticleSystem> BeamParticles;

	/* Sound */
	UPROPERTY(EditDefaultsOnly, Category = "UserClass | HitScanWeapon")
	TObjectPtr<USoundCue> ImpactSound;

	/* Multicast */
	UFUNCTION(NetMulticast, Unreliable)
	void MulticastImpact(FVector_NetQuantize ImpactPoint, FVector_NetQuantizeNormal ImpactNormal);
};
