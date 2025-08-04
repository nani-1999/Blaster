// Copyright Na9i Studio.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "BlasterAnimInstance.generated.h"

UCLASS()
class BLASTER_API UBlasterAnimInstance : public UAnimInstance
{
	GENERATED_BODY()
	
public:
	//virtual void NativeInitializeAnimation() override;
	virtual void NativeUpdateAnimation(float DeltaSeconds) override;

	/* Movement */
	UPROPERTY(BlueprintReadOnly)
	float SurfaceVelocity;

	UPROPERTY(BlueprintReadOnly)
	bool bIsAccelerating;

	UPROPERTY(BlueprintReadOnly)
	bool bIsInAir;

	UPROPERTY(BlueprintReadOnly)
	bool bIsCrouched;

	/* Combat */
	UPROPERTY(BlueprintReadOnly)
	bool bIsWeaponEquipped;

	UPROPERTY(BlueprintReadOnly)
	bool bIsAiming;

	/* Strafe */
	UPROPERTY(BlueprintReadOnly)
	float Strafe; /* Movement Angle */
	UPROPERTY(BlueprintReadOnly)
	float SmoothStrafe;

	/* Lean */
	UPROPERTY(BlueprintReadOnly)
	float Lean; /* Delta Surface Aim */
	float PreviousSurfaceAimAngle;

	/* Aim Offset */
	UPROPERTY(BlueprintReadOnly)
	float AimYaw;
	UPROPERTY(BlueprintReadOnly)
	float AimPitch;

	/* Rotate Root Bone */
	UPROPERTY(BlueprintReadOnly)
	float RootYaw;
	float StoppedSurfaceAimAingle;
	UPROPERTY(BlueprintReadOnly)
	float TurnedSurfaceAimAngle;
};
