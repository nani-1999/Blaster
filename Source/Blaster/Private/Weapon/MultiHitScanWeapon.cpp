// Copyright Na9i Studio.


#include "Weapon/MultiHitScanWeapon.h"

AMultiHitScanWeapon::AMultiHitScanWeapon() {}

void AMultiHitScanWeapon::FireBullet(const FVector& HitTarget) {

	/* we use AWeapon, instead of 'this' parent */
	AWeapon::FireBullet(HitTarget);
}