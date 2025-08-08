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
float RAInterpTo(float CurrentRotationAxis, float TargetRotationAxis, float DeltaTime, float InterpSpeed, float SMALL_NUM = UE_KINDA_SMALL_NUMBER) {
	if (InterpSpeed <= 0.f)
	{
		return TargetRotationAxis;
	}

	const float NormalizedAngle = FRotator::NormalizeAxis(TargetRotationAxis - CurrentRotationAxis);

	if (FMath::Square(NormalizedAngle) < SMALL_NUM)
	{
		return TargetRotationAxis;
	}

	const float DeltaRotationAxis = NormalizedAngle * FMath::Clamp<float>(DeltaTime * InterpSpeed, 0.f, 1.f);

	return FRotator::NormalizeAxis(CurrentRotationAxis + DeltaRotationAxis); /* normalized here also */
}

void UBlasterAnimInstance::NativeInitializeAnimation() {
	Super::NativeInitializeAnimation();
	
	ABlasterCharacter* BlasterCharacter = Cast<ABlasterCharacter>(TryGetPawnOwner());
	
	if (BlasterCharacter) {
		FRotator BaseAimRot = BlasterCharacter->GetBaseAimRotation();
		BaseAimRot.Normalize();

		/* Doing this just-in-case in spawned with a different surface angle */
		StoppedSurfaceAimAingle = BaseAimRot.Yaw;
	}
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
		StoppedSurfaceAimAingle = (SurfaceVelocity) ? SurfaceAimAngle : StoppedSurfaceAimAingle;
		/* Angle between SurfaceAimAngle and StoppedSurfaceAimAngle, StopTurnedSurfaceAimAngle specifically
		 * usually used for Root rotation while idlying 
		 * can zero out when velocity, since SurfaceAimAngle cancles itself */
		TurnedSurfaceAimAngle = FRotator::NormalizeAxis(SurfaceAimAngle - StoppedSurfaceAimAingle);
		/* TurnInPlace */
		if (TurnedSurfaceAimAngle > 90.f || TurnedSurfaceAimAngle < -90.f) bTurning = true; /* start turning */
		if (StoppedSurfaceAimAingle == SurfaceAimAngle) bTurning = false; /* won't stop until stopangle interp to actualangle */
		if (bTurning) StoppedSurfaceAimAingle = RAInterpTo(StoppedSurfaceAimAingle, SurfaceAimAngle, DeltaSeconds, 5.f, 1.f);
		/* Root 
		 * this root rotation is needed when character is using controller's yaw
		 * you just can't harness aimoffset's yaw while character is using controller's yaw, because character yaws itself instead of staying still and looking sides
		 * so we do illusionary opposite root bone rotation which makes character staying still 
		 * can test this with any zeropose 
		 * only rotating root bone on equipped */
		RootYaw = (SurfaceVelocity) ? 0.f : -TurnedSurfaceAimAngle;

		/* FABRIK
		 * getting WorldSpace Transform of Weapon's LeftHandSocket 
		 * we need to convert that WorldSpace into a BoneSpace
		 * in this case, relative to one of the CharacterMesh's Bone, eg: hand_r */
		FTransform LeftHandSocketTransform = bIsWeaponEquipped ? BlasterCharacter->GetWeaponLeftHandSocketTransform() : FTransform();

		FVector LeftHandBoneLoc;
		FRotator LeftHandBoneRot;
		BlasterCharacter->GetMesh()->TransformToBoneSpace(FName("hand_r"), LeftHandSocketTransform.GetLocation(), FRotator(0.f), LeftHandBoneLoc, LeftHandBoneRot);

		LeftHandBoneTransform.SetLocation(LeftHandBoneLoc);
		LeftHandBoneTransform.SetRotation(FQuat(LeftHandBoneRot));

		//NANI_LOG(Warning, "Stopped: %f | Turned: %f | Root: %f", StoppedSurfaceAimAingle, TurnedSurfaceAimAngle, RootYaw);
	}
}
