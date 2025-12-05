// Copyright Na9i Studio.


#include "Weapon/ProjectileRocket.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Particles/ParticleSystemComponent.h"

AProjectileRocket::AProjectileRocket() :
	MinDamage{ 20.f },
	InnerRadius{ 200.f },
	OuterRadius{ 500.f }
{
	Damage = 50.f;

	ProjectileComp->InitialSpeed = 7500.f;
	ProjectileComp->MaxSpeed = 7500.f;
}

void AProjectileRocket::OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit) {
	/* DamageCauser is Owner, which is AWeapon
	 * Instigator is Weapon's Owner which is APawn 
	 * InstigatedBy is Pawn's Controller, which is AController */

	UGameplayStatics::ApplyRadialDamageWithFalloff(this, Damage, MinDamage, GetActorLocation(), InnerRadius, OuterRadius, 1.f, UDamageType::StaticClass(), TArray<AActor*>(), GetOwner(), GetInstigatorController());

	/* calling super after, since parent class's virtual function has Destroy() */
	Super::OnHit(HitComponent, OtherActor, OtherComp, NormalImpulse, Hit);
}