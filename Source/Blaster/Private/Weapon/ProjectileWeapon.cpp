// Copyright Na9i Studio.


#include "Weapon/ProjectileWeapon.h"
#include "Weapon/Projectile.h"
#include "Engine/SkeletalMeshSocket.h"

#include "Blaster/Nani/NaniUtility.h"

AProjectileWeapon::AProjectileWeapon() {}

//
//============================================ Fire Bullet ============================================
//
void AProjectileWeapon::FireBullet(const FVector& HitTarget) {
	/* happens on Authority */
	if (ProjectileClass == nullptr) return;

	/* Weapon Muzzle Socket */
	const USkeletalMeshSocket* MuzzleSocket = WeaponMesh->GetSocketByName(FName("MuzzleFlash"));
	if (MuzzleSocket == nullptr) return;
	FTransform MuzzleSocketTransform = MuzzleSocket->GetSocketTransform(WeaponMesh);
	MuzzleSocketTransform.SetRotation(FQuat((HitTarget - MuzzleSocketTransform.GetLocation()).Rotation()));

	/* Spawning Weapon Bullet */
	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = this; /* Weapon */
	SpawnParams.Instigator = GetOwner<APawn>(); /* Weapon's Owner, which is APawn */

	AProjectile* Projectile = GetWorld()->SpawnActor<AProjectile>(ProjectileClass, MuzzleSocketTransform, SpawnParams);

	Super::FireBullet(HitTarget);
}