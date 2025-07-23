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

	/* Blueprint Readable Data */
	UPROPERTY(BlueprintReadOnly)
	float Velocity;

	UPROPERTY(BlueprintReadOnly)
	float SurfaceVelocity;

	UPROPERTY(BlueprintReadOnly)
	bool bIsInAir;

	UPROPERTY(BlueprintReadOnly)
	bool bIsAccelerating;

	UPROPERTY(BlueprintReadOnly)
	bool bIsWeaponEquipped;

	UPROPERTY(BlueprintReadOnly)
	bool bIsCrouched;

	UPROPERTY(BlueprintReadOnly)
	bool bIsAiming;

	UPROPERTY(BlueprintReadOnly)
	float Strafe;

	UPROPERTY(BlueprintReadOnly)
	float Lean;
};
