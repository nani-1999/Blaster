// Copyright Na9i Studio.


#include "Weapon/ProjectileWeapon.h"
#include "Weapon/Projectile.h"
#include "Engine/SkeletalMeshSocket.h"
#include "Components/BoxComponent.h"
#include "Interface/CombatInterface.h"
#include "BlasterComponents/CombatComponent.h"

#include "Blaster/Nani/NaniUtility.h"

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
//============================================ Fire Bullet ============================================
//
void AProjectileWeapon::FireBullet(const FVector& HitTarget) {
	/* happens on Authority */
	if (!HasAuthority() || ProjectileClass == nullptr) return;

	/* Weapon Muzzle Socket */
	const USkeletalMeshSocket* MuzzleSocket = WeaponMesh->GetSocketByName(FName("MuzzleFlash"));
	if (MuzzleSocket == nullptr) return;
	FTransform MuzzleSocketTransform = MuzzleSocket->GetSocketTransform(WeaponMesh);
	MuzzleSocketTransform.SetRotation(FQuat((HitTarget - MuzzleSocketTransform.GetLocation()).Rotation()));

	/* Spawning Weapon Bullet */
	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = this; /* weapon */
	SpawnParams.Instigator = GetOwner<APawn>(); /* weapon's owner which is APawn */

	AProjectile* Projectile = GetWorld()->SpawnActor<AProjectile>(ProjectileClass, MuzzleSocketTransform, SpawnParams);
	//Projectile->GetBoxComp()->OnComponentHit.AddDynamic(this, &AProjectileWeapon::OnProjectileHit);
}

//void AProjectileWeapon::OnProjectileHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit) {
//	/* happens on Authority */
//	if (ICombatInterface* CombatInterface = Cast<ICombatInterface>(OtherActor)) {
//		UCombatComponent* CombatComp = CombatInterface->GetCombatComponent();
//		if (CombatComp) CombatComp->MulticastHit();
//	}
//}