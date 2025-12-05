// Copyright Na9i Studio.


#include "Weapon/ProjectileBullet.h"
//#include "Kismet/GameplayStatics.h"

AProjectileBullet::AProjectileBullet()
{
	Damage = 20.f;
}

void AProjectileBullet::OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit) {
	/* Owner is Weapon 
	 * Instigator is Weapon's Owner which is APawn */

	/* peek UGameplayStatics::ApplyDamage() to know more
	 * FDamageEvent general struct is just to calculate ActualDamage amount from base Damage
     * FDamageEvent also supports impulse i think */
	if (OtherActor && (Damage > 0.f)) {
		UDamageType const* const DamageTypeCDO = GetDefault<UDamageType>();
		AController* InstigatedBy = GetInstigatorController(); /* since its asking for a controller */
		OtherActor->OnTakeAnyDamage.Broadcast(OtherActor, Damage, DamageTypeCDO, InstigatedBy, GetOwner());
		/* must broadcast controller's delegate also
		 * since controller has delegate, to know if that controller had instigated any damage */
		if (InstigatedBy != nullptr) {
			InstigatedBy->OnInstigatedAnyDamage.Broadcast(Damage, DamageTypeCDO, OtherActor, GetOwner());
		}
	}
	//UGameplayStatics::ApplyDamage(OtherActor, Damage, GetInstigatorController(), GetOwner(), UDamageType::StaticClass());

	/* since parent class's virtual function calls Destroy() */
	Super::OnHit(HitComponent, OtherActor, OtherComp, NormalImpulse, Hit);
}
