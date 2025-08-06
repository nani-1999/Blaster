// Copyright Na9i Studio.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "CombatComponent.generated.h"

class AWeapon;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class BLASTER_API UCombatComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UCombatComponent();

	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

protected:
	virtual void BeginPlay() override;

	/* Character Movement */
	void SetOrientRotationToMovement(bool bOrient);

	/* Weapon */
	UPROPERTY(ReplicatedUsing = OnRep_EquippedWeapon)
	TObjectPtr<AWeapon> EquippedWeapon;
	UFUNCTION()
	void OnRep_EquippedWeapon(AWeapon* OldEquippedWeapon);

	/* Walk Speed */
	void SetWalkSpeed(float WalkSpeedToSet);
	UPROPERTY(EditDefaultsOnly)
	float BaseWalkSpeed;
	UPROPERTY(EditDefaultsOnly)
	float AimWalkSpeed;

	/* Aim */
	UPROPERTY(ReplicatedUsing = OnRep_Aiming)
	bool bAiming;
	UFUNCTION()
	void OnRep_Aiming(bool OldAiming);

public:
	/* Weapon */
	void EquipWeapon(AWeapon* WeaponToEquip);

	/* Getters */
	FORCEINLINE AWeapon* GetEquippedWeapon() const { return EquippedWeapon; }
	FORCEINLINE bool IsAiming() const { return bAiming; }
	FORCEINLINE float GetBaseWalkSpeed() const { return BaseWalkSpeed; }

	/* Aim */
	void SetAiming(bool bIsAiming);
};
