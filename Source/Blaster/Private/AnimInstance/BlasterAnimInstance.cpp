// Copyright Na9i Studio.


#include "AnimInstance/BlasterAnimInstance.h"
#include "Character/BlasterCharacter.h"

#include "Blaster/Nani/NaniUtility.h"

float NormalizeRotationAxis(float Angle) {
	if (Angle <= 180.f && Angle >= -180.f) return Angle;

	int IntOfAngle = (int)Angle;
	float DecimalOfAngle = Angle - IntOfAngle;

	float Normalized = (IntOfAngle % 360) + DecimalOfAngle;
	if (Normalized > 180.f) return Normalized - 360.f;
	if (Normalized < -180.f) return Normalized + 360.f;
	return Normalized;
}
float RAInterpTo(float CurrentRotationAxis, float TargetRotationAxis, float DeltaTime, float InterpSpeed) {
	if (InterpSpeed <= 0.f)
	{
		return TargetRotationAxis;
	}

	const float NormalizedAngle = NormalizeRotationAxis(TargetRotationAxis - CurrentRotationAxis);

	if (FMath::Square(NormalizedAngle) < SMALL_NUMBER)
	{
		return TargetRotationAxis;
	}

	const float DeltaRotationAxis = NormalizedAngle * FMath::Clamp<float>(DeltaTime * InterpSpeed, 0.f, 1.f);

	return NormalizeRotationAxis(CurrentRotationAxis + DeltaRotationAxis); /* normalized here also */
}

void UBlasterAnimInstance::NativeUpdateAnimation(float DeltaSeconds) {
	Super::NativeUpdateAnimation(DeltaSeconds);

	ABlasterCharacter* BlasterCharacter = Cast<ABlasterCharacter>(TryGetPawnOwner());

	if (BlasterCharacter) {
		FVector Velocity3D = BlasterCharacter->GetVelocity();

		SurfaceVelocity = Velocity3D.Size2D();
		bIsInAir = BlasterCharacter->IsInAir();
		bIsAccelerating = BlasterCharacter->IsAccelerating();
		bIsWeaponEquipped = BlasterCharacter->IsWeaponEquipped();
		bIsCrouched = BlasterCharacter->bIsCrouched;
		bIsAiming = BlasterCharacter->IsAiming();

		/* Strafe */
		float SurfaceAimAngle = BlasterCharacter->GetActorRotation().Yaw;
		float SurfaceVelocityAngle = FMath::Atan2(Velocity3D.Y, Velocity3D.X) * (180.f / PI);
		Strafe = (SurfaceVelocity) ? NormalizeRotationAxis(SurfaceVelocityAngle - SurfaceAimAngle) : 0.f;
		SmoothStrafe = (SurfaceVelocity) ? RAInterpTo(SmoothStrafe, Strafe, DeltaSeconds, 5.f) : 0.f;

		/* Lean */
		Lean = NormalizeRotationAxis(SurfaceAimAngle - PreviousSurfaceAimAngle) * 5.f;
		PreviousSurfaceAimAngle = SurfaceAimAngle;

		//NANI_LOG(Warning, "Lean: %f | Strafe: %f | ActorRotation: %s", Lean, Strafe, *BlasterCharacter->GetActorRotation().ToString());
	}
}
