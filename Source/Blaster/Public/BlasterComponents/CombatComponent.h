// Copyright Na9i Studio.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Weapon/WeaponTypes.h"
//#include "CombatTypes.h"
#include "CombatComponent.generated.h"

class AWeapon;
class UAnimMontage;
class UCameraComponent;
class ABlasterPlayerController;

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
	void UpdateHUDCrosshair(float DeltaTime);
	float CrosshairSurfaceFactor;
	float CrosshairInAirFactor;
	float CrosshairAimFactor;

	/* Field Of View */
	UPROPERTY(EditDefaultsOnly)
	float BaseFOV;
	float InterpedFOV;
	UCameraComponent* OwnerCharCamera; /* just using raw ptr since comp having other actor's comp is weird */

	/* HitScan */
	FHitResult CursorHitResult;
	FVector CursorEndPosition;
	void TraceUnderCursor(float TraceLength = 5000.f, bool bOffset = true);

	/* Montage */
	void PlayCharacterMontage(UAnimMontage* MontageToPlay, FName SectionName);

	/* Fire */
	bool bFirePressed; /* Fire Buttom Pressed */
	void FireWeapon();
	UFUNCTION(Server, Reliable)
	void ServerFire(const FVector_NetQuantize HitTarget);
	UFUNCTION(NetMulticast, Unreliable)
	void MulticastFire();
	UFUNCTION(Server, Unreliable)
	void ServerFireEmpty();
	UFUNCTION(NetMulticast, Unreliable)
	void MulticastFireEmpty();
	/* Fire Timers */
	bool bAllowFire;
	FTimerHandle AllowFireTimerHandle;
	void AllowFire();
	/* Fire Montage */
	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<UAnimMontage> FireMontage;
	void PlayCharacterFireMontage();

	/* Ammo */
	UPROPERTY()
	TMap<EWeaponType, int32> AllCarriedAmmo; /* Carried Ammo of All Weapon Types */
	void InitAllCarriedAmmo(int InitVal);
	UPROPERTY(ReplicatedUsing = OnRep_CarriedAmmo)
	int32 CarriedAmmo; /* Carried Ammo of Equipped Weapon Type */
	UFUNCTION()
	void OnRep_CarriedAmmo();

	/* Combat */
	//UPROPERTY(ReplicatedUsing = OnRep_CombatType)
	//ECombatType CombatType;
	//UFUNCTION()
	//void OnRep_CombatType();

	/* Reload Timer */
	//FTimerHandle ReloadTimer;
	//void ReloadFinished();

	/* References */
	ACharacter* OwnerChar = nullptr;
	ABlasterPlayerController* BlasterPC = nullptr;

public:
	/* Weapon */
	void EquipWeapon(AWeapon* WeaponToEquip);
	FTransform GetWeaponGripSocket() const;
	void DropWeapon();

	/* Getters */
	//FORCEINLINE AWeapon* GetEquippedWeapon() const { return EquippedWeapon; }
	FORCEINLINE bool IsWeaponEquipped() const { return !(EquippedWeapon == nullptr); }
	FORCEINLINE bool IsAiming() const { return bAiming; }
	FORCEINLINE float GetBaseWalkSpeed() const { return BaseWalkSpeed; }

	/* Ammo */
	//int32 GetWeaponAmmoCapacity() const;
	int32 GetWeaponAmmo() const;
	FORCEINLINE int32 GetCarriedAmmo() const { return EquippedWeapon ? CarriedAmmo : 0; }

	/* Aim */
	void SetAiming(bool bIsAiming);

	/* Fire */
	void SetFiring(bool bPressed);

	/* Field Of View */
	FORCEINLINE void SetCamera(UCameraComponent* Camera) { OwnerCharCamera = Camera; }

	/* Combat */
	//FORCEINLINE ECombatType GetCombatType() const { return CombatType; }

	/* Reload */
	//UFUNCTION(Server, Reliable)
	//void ServerReloadPressed();

	/* References */
	void SetReferences();
};
