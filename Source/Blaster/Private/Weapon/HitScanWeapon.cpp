// Copyright Na9i Studio.


#include "Weapon/HitScanWeapon.h"
#include "Engine/SkeletalMeshSocket.h"
#include "Interface/CombatInterface.h"
#include "Kismet/GameplayStatics.h"
#include "Particles/ParticleSystem.h"
#include "Particles/ParticleSystemComponent.h"
#include "Sound/SoundCue.h"

#include "DrawDebugHelpers.h"
#include "Blaster/Nani/NaniUtility.h"

AHitScanWeapon::AHitScanWeapon() :
	Damage{ 10.f },
	PelletsPerBullet{ 1 },
	bScatter{ false },
	ScatterSize{ 200.f }
{

}

void AHitScanWeapon::FireBullet(const FVector& HitTarget) {
	/* Happens on Authority */

	APawn* OwnerPawn = GetOwner<APawn>();
	if (OwnerPawn == nullptr || OwnerPawn->Controller == nullptr) return;

	const USkeletalMeshSocket* MuzzleSocket = WeaponMesh->GetSocketByName(FName("MuzzleFlash"));

	if (MuzzleSocket) {
		FTransform MuzzleSocketTransform = MuzzleSocket->GetSocketTransform(WeaponMesh);

		FVector StartPoint = MuzzleSocketTransform.GetLocation();
		FVector ToEndPoint = (HitTarget - StartPoint).GetSafeNormal(); /* direction towards endpoint */

		/* Pellet per Bullet / Traces per Round */
		for (int i = 0; i < PelletsPerBullet; i++) {
			/* Scatter or else Non-Scatter */
			if (bScatter) {
				FVector ScatterStartPoint = StartPoint + ToEndPoint * 800.f;
				//DrawDebugSphere(GetWorld(), ScatterStartPoint, ScatterSize, 12.f, FColor::Purple, false, 6.f, 0, 4.f);
				FVector RandScatterPoint = ScatterStartPoint + FMath::VRand() * ScatterSize;
				//DrawDebugPoint(GetWorld(), RandScatterPoint, 6.f, FColor::Cyan, false, 6.f);
				FVector ToRandScatterPoint = (RandScatterPoint - StartPoint).GetSafeNormal();
				FVector RandScatterEndPoint = StartPoint + ToRandScatterPoint * 5000.f;

				TraceAndDamage(StartPoint, RandScatterEndPoint, OwnerPawn);
			}
			else {
				FVector EndPoint = StartPoint + ToEndPoint * 5000.f;

				TraceAndDamage(StartPoint, EndPoint, OwnerPawn);
			}
		}
	}

	Super::FireBullet(HitTarget);
}

void AHitScanWeapon::TraceAndDamage(FVector StartPoint, FVector EndPoint, APawn* InstigatorPawn) {
	if (UWorld* World = GetWorld()) {
		FHitResult HitResult;
		World->LineTraceSingleByChannel(HitResult, StartPoint, EndPoint, ECollisionChannel::ECC_Visibility);

		//FColor DebugColor = FColor::Red;

		if (HitResult.bBlockingHit) {
			ICombatInterface* CombatInterface = Cast<ICombatInterface>(HitResult.GetActor());
			/* apply damage to actor with has combat interface */
			if (CombatInterface) {
				UGameplayStatics::ApplyDamage(HitResult.GetActor(), Damage, InstigatorPawn->Controller, this, UDamageType::StaticClass());

				//DebugColor = FColor::Green;
				//DrawDebugSphere(GetWorld(), HitResult.ImpactPoint, 10.f, 8.f, FColor::Green, false, 6.f, 0, 4.f);
			}
		}

		//DrawDebugLine(GetWorld(), StartPoint, HitResult.bBlockingHit ? HitResult.ImpactPoint : EndPoint, DebugColor, false, 6.f, 0, 4.f);

		/* Multicast Impact Particle and Sound */
		MulticastImpact(HitResult.bBlockingHit ? HitResult.ImpactPoint : EndPoint, HitResult.ImpactNormal);
	}
}

void AHitScanWeapon::MulticastImpact_Implementation(FVector_NetQuantize ImpactPoint, FVector_NetQuantizeNormal ImpactNormal) {
	/* this function is solely for doing particle and sounds, and is unreliable */

	/* impact particle */
	if (ImpactParticles) UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ImpactParticles, ImpactPoint, ImpactNormal.Rotation());
	/* Beam Particle */
	if (BeamParticles) {
		const USkeletalMeshSocket* MuzzleSocket = WeaponMesh->GetSocketByName(FName("MuzzleFlash"));
		if (MuzzleSocket) {
			FTransform MuzzleSocketTransform = MuzzleSocket->GetSocketTransform(WeaponMesh);
			UParticleSystemComponent* BeamParticleComp = UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), BeamParticles, MuzzleSocketTransform);
			if (BeamParticleComp) BeamParticleComp->SetVectorParameter(FName("Target"), ImpactPoint);
		}
	}
	/* Impact Sound */
	if (ImpactSound) UGameplayStatics::PlaySoundAtLocation(this, ImpactSound, ImpactPoint);
}