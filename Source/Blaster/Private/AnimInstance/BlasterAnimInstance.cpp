// Copyright Na9i Studio.


#include "AnimInstance/BlasterAnimInstance.h"
#include "Character/BlasterCharacter.h"

#include "Blaster/Nani/NaniUtility.h"

float NormalizeRotationAxis(float Angle) {
	if (Angle > 180.f) Angle -= 360.f;
	if (Angle < -180.f) Angle += 360.f;
	return Angle;
}

void UBlasterAnimInstance::NativeUpdateAnimation(float DeltaSeconds) {
	Super::NativeUpdateAnimation(DeltaSeconds);

	ABlasterCharacter* BlasterCharacter = Cast<ABlasterCharacter>(TryGetPawnOwner());

	if (BlasterCharacter) {
		FVector Velocity3D = BlasterCharacter->GetVelocity();

		Velocity = Velocity3D.Size();
		SurfaceVelocity = Velocity3D.Size2D();
		bIsInAir = BlasterCharacter->IsInAir();
		bIsAccelerating = BlasterCharacter->IsAccelerating();
		bIsWeaponEquipped = BlasterCharacter->IsWeaponEquipped();
		bIsCrouched = BlasterCharacter->bIsCrouched;
		bIsAiming = BlasterCharacter->IsAiming();

		float SurfaceAimAngle = BlasterCharacter->GetActorRotation().Yaw;
		float SurfaceVelocityAngle = FMath::Atan2(Velocity3D.Y, Velocity3D.X) * (180.f / PI);
		Strafe = (Velocity) ? NormalizeRotationAxis(SurfaceVelocityAngle - SurfaceAimAngle) : 0.f;

		NANI_LOG(Warning, "Aim: %f | Velocity: %f | Diff: %f", SurfaceAimAngle, SurfaceVelocityAngle, Strafe);
	}
}