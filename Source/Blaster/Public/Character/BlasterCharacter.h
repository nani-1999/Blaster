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

class UStaticMeshComponent;
class USkeletalMeshComponent;

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
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UStaticMeshComponent> TestMesh;
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<USkeletalMeshComponent> WeaponMesh;

	UFUNCTION()
	void TestPressed();
	UFUNCTION(Server, Reliable)
	void ServerTestPressed();

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
	virtual UCombatComponent* GetCombatComponent() const override { return Combat; }

public:
	/* Weapon */
	void SetOverlappingWeapon(AWeapon* WeaponToSet);
	FTransform GetWeaponLeftHandSocketTransform() const;

	/* Getters */
	bool IsInAir();
	bool IsAccelerating();
	FORCEINLINE AWeapon* GetOverlappingWeapon() const { return OverlappingWeapon; }
	bool IsWeaponEquipped();
	bool IsAiming();
};
