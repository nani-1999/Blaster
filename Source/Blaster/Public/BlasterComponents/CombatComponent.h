// Copyright Na9i Studio.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "CombatComponent.generated.h"

class AWeapon;
class UAnimMontage;
class UCameraComponent;

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

	/* Crosshair */
	void UpdateHUDCrosshair(ACharacter* CompOwner, bool bIsInAir, float DeltaTime);
	float CrosshairSurfaceFactor;
	float CrosshairInAirFactor;
	float CrosshairAimFactor;

	/* Field Of View */
	UPROPERTY(EditDefaultsOnly)
	float BaseFOV;
	float InterpedFOV;
	UCameraComponent* CompOwnerCamera; /* just using raw ptr since comp having other actor's comp is weird */

	/* HitScan */
	void TraceUnderCursor(FHitResult& OutHitResult, FVector& EndPoint, float TraceLength = 5000.f, bool bOffset = true);

	/* Fire */
	UFUNCTION(Server, Reliable)
	void ServerFire(const FVector_NetQuantize HitTarget);
	UFUNCTION(NetMulticast, Unreliable)
	void MulticastFire();

	//UPROPERTY(ReplicatedUsing = OnRep_Firing)
	//bool bFiring;
	//UFUNCTION()
	//void OnRep_Firing(bool OldFiring);

	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<UAnimMontage> FireMontage;
	void PlayCharacterFireMontage();

public:
	/* Weapon */
	void EquipWeapon(AWeapon* WeaponToEquip);
	FTransform GetWeaponLeftHandSocketTransform() const;

	/* Getters */
	FORCEINLINE AWeapon* GetEquippedWeapon() const { return EquippedWeapon; }
	FORCEINLINE bool IsAiming() const { return bAiming; }
	FORCEINLINE float GetBaseWalkSpeed() const { return BaseWalkSpeed; }

	/* Aim */
	void SetAiming(bool bIsAiming);

	/* Fire */
	void SetFiring(bool bIsFiring);

	/* Setters */
	FORCEINLINE void SetCamera(UCameraComponent* Camera) { CompOwnerCamera = Camera; }

	/* HIt */
	UFUNCTION(NetMulticast, Unreliable)
	void MulticastHit();
};
