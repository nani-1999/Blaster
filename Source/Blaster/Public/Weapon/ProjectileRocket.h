// Copyright Na9i Studio.

#pragma once

#include "CoreMinimal.h"
#include "Weapon/Projectile.h"
#include "ProjectileRocket.generated.h"

class UParticleSystemComponent;

UCLASS()
class BLASTER_API AProjectileRocket : public AProjectile
{
	GENERATED_BODY()
	
public:
	AProjectileRocket();

protected:
	virtual void OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit) override;

	/* just adding it bp, since we don't use it on runtime */
	//UPROPERTY(EditDefaultsOnly, Category = "UserClass | ProjectileRocket")
	//TObjectPtr<UParticleSystemComponent> TrailParticle;

	/* Stats */
	UPROPERTY(EditDefaultsOnly, Category = "UserClass | ProjectileRocket")
	float MinDamage;
	UPROPERTY(EditDefaultsOnly, Category = "UserClass | ProjectileRocket")
	float InnerRadius;
	UPROPERTY(EditDefaultsOnly, Category = "UserClass | ProjectileRocket")
	float OuterRadius;
};
