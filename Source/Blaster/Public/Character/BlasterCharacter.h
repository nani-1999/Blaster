// Copyright Na9i Studio.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Interface/CombatInterface.h"
#include "BlasterCharacter.generated.h"

class USpringArmComponent;
class UCameraComponent;
class AWeapon;
class UWidgetComponent;
class UCombatComponent;
//class UMaterialInstance;

UCLASS()
class BLASTER_API ABlasterCharacter : public ACharacter, public ICombatInterface
{
	GENERATED_BODY()

public:
	ABlasterCharacter();

	virtual void Tick(float DeltaTime) override;

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	virtual void PossessedBy(AController* NewController) override;
	virtual void OnRep_PlayerState() override;

	/* Property Registerer For Replication */
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	//virtual void PostInitializeComponents() override;

protected:
	virtual void BeginPlay() override;

	/* Input */
	UFUNCTION()
	void MoveForward(const float Value);
	UFUNCTION()
	void MoveRight(const float Value);
	UFUNCTION()
	void LookUp(const float Value);
	UFUNCTION()
	void Turn(const float Value);

	UFUNCTION()
	void EquipPressed();
	UFUNCTION(Server, Reliable)
	void ServerEquipPressed();

	UFUNCTION()
	void FirePressed();
	UFUNCTION()
	void FireReleased();

	UFUNCTION()
	void CrouchPressed();

	virtual	void Jump() override;

	UFUNCTION()
	void AimPressed();
	UFUNCTION(Server, Reliable)
	void ServerAimPressed();
	UFUNCTION()
	void AimReleased();
	UFUNCTION(Server, Reliable)
	void ServerAimReleased();

	/* Test */
	UFUNCTION()
	void TestPressed();
	UFUNCTION(Server, Reliable)
	void ServerTestPressed();
	UFUNCTION(NetMulticast, Reliable)
	void MulticastTestPressed();

	/* Camera */
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<USpringArmComponent> CameraBoom;
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UCameraComponent> FollowCamera;

	/* Overhead Widget */
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UWidgetComponent> OverheadWidget;
	void SetupOverheadWidget();

	/* Weapon */
	UPROPERTY(ReplicatedUsing = OnRep_OverlappingWeapon)
	TObjectPtr<AWeapon> OverlappingWeapon;
	UFUNCTION()
	void OnRep_OverlappingWeapon(AWeapon* OldWeapon) const;

	/* Combat */
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UCombatComponent> Combat;

	/* Combat Interface */
	virtual UCombatComponent* GetCombatComponent() const override { return Combat; } /* override */

	/* Play Montage */
	void PlayMontage(UAnimMontage* MontageToPlay, FName SectionName);

	/* Stats */
	UPROPERTY(VisibleAnywhere)
	float MaxHealth;
	UPROPERTY(ReplicatedUsing = OnRep_CurrentHealth)
	float CurrentHealth;
	UFUNCTION()
	void OnRep_CurrentHealth(float OldCurrentHealth);

	/* Damage */
	UFUNCTION()
	void TakenAnyDamage(AActor* DamagedActor, float Damage, const UDamageType* DamageType, AController* InstigatedBy, AActor* DamageCauser);

	/* Hit */
	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<UAnimMontage> HitReactMontage;
	//void PlayHitReactMontage();

	/* Elimination */
	bool bEliminated;
	UFUNCTION(NetMulticast, Reliable)
	void MulticastEliminated();
	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<UAnimMontage> ElimMontage;
	/* Timer */
	float ElimAnimTime;
	FTimerHandle ElimAnimTimerHandle;
	void ElimAnimFinished();

	/* Timeline */
	UPROPERTY(VisibleAnywhere)
	class UTimelineComponent* Transition;

	/* Dissolve Material */
	void DissolveMaterial();
	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<UMaterialInstance> DissolveMaterialInstance;
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UMaterialInstanceDynamic> DissolveMaterialInstanceDynamic;
	UPROPERTY(EditDefaultsOnly)
	class UCurveFloat* DissolveCurve;
	UFUNCTION()
	void UpdateDissolveMaterial(float Value);

public:
	/* Weapon */
	void SetOverlappingWeapon(AWeapon* WeaponToSet);
	FTransform GetWeaponLeftHandSocketTransform() const;

	/* Elimination */
	void Eliminated();
	FORCEINLINE bool IsEliminated() const { return bEliminated; }

	/* Getters */
	bool IsInAir();
	bool IsAccelerating();
	FORCEINLINE AWeapon* GetOverlappingWeapon() const { return OverlappingWeapon; }
	bool IsWeaponEquipped();
	bool IsAiming();
};
