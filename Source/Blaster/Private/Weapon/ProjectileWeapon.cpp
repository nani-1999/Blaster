// Copyright Na9i Studio.


#include "Weapon/ProjectileWeapon.h"
#include "Weapon/Projectile.h"
#include "Engine/SkeletalMeshSocket.h"


AProjectileWeapon::AProjectileWeapon() {
	PrimaryActorTick.bCanEverTick = false;

}

void AProjectileWeapon::Tick(float DeltaTime) {
	Super::Tick(DeltaTime);

}

void AProjectileWeapon::BeginPlay() {
	Super::BeginPlay();

}

//
//============================================ Fire ============================================
//
void AProjectileWeapon::Fire(const FVector& ProjectileHitTarget) {
	Super::Fire(ProjectileHitTarget);

	if (ProjectileClass == nullptr || !HasAuthority()) return;

	/* Weapon Muzzle Socket */
	const USkeletalMeshSocket* MuzzleSocket = WeaponMesh->GetSocketByName(FName("MuzzleFlash"));
	if (MuzzleSocket == nullptr) return;
	FTransform MuzzleSocketTransform = MuzzleSocket->GetSocketTransform(WeaponMesh);
	MuzzleSocketTransform.SetRotation(FQuat((ProjectileHitTarget - MuzzleSocketTransform.GetLocation()).Rotation()));

	/* Spawning Weapon Bullet */
	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = this; /* this weapon */
	SpawnParams.Instigator = GetOwner<APawn>(); /* this weapon's owner */

	GetWorld()->SpawnActor<AProjectile>(ProjectileClass, MuzzleSocketTransform, SpawnParams);
}