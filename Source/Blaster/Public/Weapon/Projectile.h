// Copyright Na9i Studio.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Projectile.generated.h"

class UBoxComponent;
class UProjectileMovementComponent;
class UParticleSystemComponent;
class UParticleSystem;
class USoundCue;

UCLASS()
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

	/* Particle Component */
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UParticleSystemComponent> TracerParticle;

	/* Particles */
	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<UParticleSystem> ImpactParticle;

	/* Sounds */
	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<USoundCue> ImpactSound;
	
	/* Events */
	UFUNCTION()
	void OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

public:
	/* Getters */
	FORCEINLINE UBoxComponent* GetBoxComp() const { return BoxCollision; }
};
