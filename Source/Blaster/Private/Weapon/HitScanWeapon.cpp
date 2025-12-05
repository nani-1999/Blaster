// Copyright Na9i Studio.


#include "Weapon/HitScanWeapon.h"
#include "Engine/SkeletalMeshSocket.h"
#include "Interface/CombatInterface.h"
#include "Kismet/GameplayStatics.h"
#include "Particles/ParticleSystem.h"
#include "Sound/SoundCue.h"

#include "DrawDebugHelpers.h"
#include "Blaster/Nani/NaniUtility.h"

AHitScanWeapon::AHitScanWeapon() :
	Damage{ 10.f }
{}

void AHitScanWeapon::FireBullet(const FVector& HitTarget) {
	/* Happens on Authority */

	APawn* OwnerPawn = GetOwner<APawn>();
	if (OwnerPawn == nullptr || OwnerPawn->Controller == nullptr) return;

	const USkeletalMeshSocket* MuzzleSocket = WeaponMesh->GetSocketByName(FName("MuzzleFlash"));

	if (MuzzleSocket) {
		FTransform MuzzleSocketTransform = MuzzleSocket->GetSocketTransform(WeaponMesh);

		FVector StartPoint = MuzzleSocketTransform.GetLocation();
		FVector EndPoint = HitTarget + (HitTarget - StartPoint) * 0.1f; /* adding 10% of vector difference, idk we can clearly get unit vector towards hittarget but we are trying to reduce computation here */
		
		//DrawDebugSphere(GetWorld(), StartPoint, 20.f, 12.f, FColor::Green, true, 20.f, 0, 5.f);
		//DrawDebugSphere(GetWorld(), EndPoint, 20.f, 12.f, FColor::Red, true, 20.f, 0, 5.f);
		//DrawDebugLine(GetWorld(), StartPoint, EndPoint, FColor::Blue, true, 20.f, 0, 5.f);

		FHitResult HitResult;
		if (UWorld* World = GetWorld()) {
			World->LineTraceSingleByChannel(HitResult, StartPoint, EndPoint, ECollisionChannel::ECC_Visibility);
		}
		if (HitResult.bBlockingHit) {
			ICombatInterface* CombatInterface = Cast<ICombatInterface>(HitResult.GetActor());
			/* apply damage to actor with has combat interface */
			if (CombatInterface) {
				UGameplayStatics::ApplyDamage(HitResult.GetActor(), Damage, OwnerPawn->Controller, this, UDamageType::StaticClass());
			}
			
			/* Multicast Impact Particle and Sound */
			MulticastImpact(HitResult.ImpactPoint, HitResult.ImpactNormal);
		}
	}

	Super::FireBullet(HitTarget);
}

void AHitScanWeapon::MulticastImpact_Implementation(FVector_NetQuantize ImpactPoint, FVector_NetQuantizeNormal ImpactNormal) {
	/* impact particle */
	if (ImpactParticle) UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ImpactParticle, ImpactPoint, ImpactNormal.Rotation());
	/* Impact Sound */
	if (ImpactSound) UGameplayStatics::PlaySoundAtLocation(this, ImpactSound, ImpactPoint);
}