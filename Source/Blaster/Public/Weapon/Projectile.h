// Copyright Na9i Studio.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Projectile.generated.h"

class UBoxComponent;
class UProjectileMovementComponent;
class UParticleSystem;
class USoundCue;

UCLASS(Abstract)
class BLASTER_API AProjectile : public AActor
{
	GENERATED_BODY()
	
public:
	AProjectile();

	virtual void Tick(float DeltaTime) override;

protected:
	virtual void BeginPlay() override;

	/* Collision */
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UBoxComponent> BoxCollision;

	/* Projectile */
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UProjectileMovementComponent> ProjectileComp;

	/* Particles */
	UPROPERTY(EditDefaultsOnly, Category = "UserClass | Projectile")
	TObjectPtr<UParticleSystem> ImpactParticle;

	/* Sounds */
	UPROPERTY(EditDefaultsOnly, Category = "UserClass | Projectile")
	TObjectPtr<USoundCue> ImpactSound;
	
	/* Stats */
	UPROPERTY(EditDefaultsOnly, Category = "UserClass | Projectile")
	float Damage;

	/* Events */
	UFUNCTION()
	virtual void OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

	/* Destroyed */
	virtual void Destroyed() override;
};
