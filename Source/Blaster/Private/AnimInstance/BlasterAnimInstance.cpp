// Copyright Na9i Studio.


#include "AnimInstance/BlasterAnimInstance.h"
#include "Character/BlasterCharacter.h"

#include "Blaster/Nani/NaniUtility.h"

/*
float NormalizeRotationAxis(float Angle) {
	if (Angle <= 180.f && Angle >= -180.f) return Angle;

	int IntOfAngle = (int)Angle;
	float DecimalOfAngle = Angle - IntOfAngle;

	float Normalized = (IntOfAngle % 360) + DecimalOfAngle;
	if (Normalized > 180.f) return Normalized - 360.f;
	if (Normalized < -180.f) return Normalized + 360.f;
	return Normalized;
}
*/
float RAInterpTo(float CurrentRotationAxis, float TargetRotationAxis, float DeltaTime, float InterpSpeed) {
	if (InterpSpeed <= 0.f)
	{
		return TargetRotationAxis;
	}

	const float NormalizedAngle = FRotator::NormalizeAxis(TargetRotationAxis - CurrentRotationAxis);

	if (FMath::Square(NormalizedAngle) < SMALL_NUMBER)
	{
		return TargetRotationAxis;
	}

	const float DeltaRotationAxis = NormalizedAngle * FMath::Clamp<float>(DeltaTime * InterpSpeed, 0.f, 1.f);

	return FRotator::NormalizeAxis(CurrentRotationAxis + DeltaRotationAxis); /* normalized here also */
}

void UBlasterAnimInstance::NativeUpdateAnimation(float DeltaSeconds) {
	Super::NativeUpdateAnimation(DeltaSeconds);

	ABlasterCharacter* BlasterCharacter = Cast<ABlasterCharacter>(TryGetPawnOwner());

	if (BlasterCharacter) {
		FVector Velocity3D = BlasterCharacter->GetVelocity();
		FRotator BaseAimRot = BlasterCharacter->GetBaseAimRotation();
		BaseAimRot.Normalize(); /* must be normalized since the axis are packet compressed on network */

		SurfaceVelocity = Velocity3D.Size2D();
		bIsInAir = BlasterCharacter->IsInAir();
		bIsAccelerating = BlasterCharacter->IsAccelerating();
		bIsWeaponEquipped = BlasterCharacter->IsWeaponEquipped();
		bIsCrouched = BlasterCharacter->bIsCrouched;
		bIsAiming = BlasterCharacter->IsAiming();

		/* Strafe 
		 * since smoothing strafe in blend space giving normalization issue, we did it here */
		float SurfaceAimAngle = BaseAimRot.Yaw;
		float SurfaceVelocityAngle = FMath::Atan2(Velocity3D.Y, Velocity3D.X) * (180.f / PI);
		Strafe = (SurfaceVelocity) ? FRotator::NormalizeAxis(SurfaceVelocityAngle - SurfaceAimAngle) : 0.f;
		SmoothStrafe = (SurfaceVelocity) ? RAInterpTo(SmoothStrafe, Strafe, DeltaSeconds, 5.f) : 0.f;

		/* Lean 
		 * for smoothing lean, we did that in blend space
		 * smooth lean, since lean is very noisy on simulated proxy */
		Lean = FRotator::NormalizeAxis(SurfaceAimAngle - PreviousSurfaceAimAngle) * 5.f;
		PreviousSurfaceAimAngle = SurfaceAimAngle;

		/* Aim Offset */
		AimYaw = BaseAimRot.Yaw;
		AimPitch = BaseAimRot.Pitch;

		/* SurfaceAimAngle when SurfaceVelocity is Zero */
		StoppedSurfaceAimAingle = SurfaceVelocity ? SurfaceAimAngle : StoppedSurfaceAimAingle;
		/* Angle between SurfaceAimAngle and StoppedSurfaceAimAngle, StopTurnedSurfaceAimAngle specifically
		 * usually used for Root rotation while idlying */
		TurnedSurfaceAimAngle = FRotator::NormalizeAxis(SurfaceAimAngle - StoppedSurfaceAimAingle);
		/* Root */
		RootYaw = SurfaceVelocity ? 0.f : -TurnedSurfaceAimAngle;

		//int TurnDirection = (Lean > 0.f) ? 1 : (Lean < 0.f) ? -1 : 0;
		//NANI_LOG(Warning, "AimYaw: %f | RootYaw: %f | Stopped: %f | Turned: %f", AimPitch, RootYaw, StoppedSurfaceAimAingle, TurnedSurfaceAimAngle);
	}
}
