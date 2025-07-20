// Copyright Na9i Studio.


#include "AnimInstance/BlasterAnimInstance.h"
#include "Character/BlasterCharacter.h"

void UBlasterAnimInstance::NativeUpdateAnimation(float DeltaSeconds) {
	Super::NativeUpdateAnimation(DeltaSeconds);

	ABlasterCharacter* BlasterCharacter = Cast<ABlasterCharacter>(TryGetPawnOwner());

	if (BlasterCharacter) {
		Velocity = BlasterCharacter->GetVelocity().Size2D();
		bIsInAir = BlasterCharacter->IsInAir();
		bIsAccelerating = BlasterCharacter->IsAccelerating();
		bIsWeaponEquipped = BlasterCharacter->IsWeaponEquipped();
		bIsCrouched = BlasterCharacter->bIsCrouched;
	}
}