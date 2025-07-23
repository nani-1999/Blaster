// Copyright Na9i Studio.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "BlasterCharacter.generated.h"

class USpringArmComponent;
class UCameraComponent;
class AWeapon;
class UWidgetComponent;
class UCombatComponent;

class UStaticMeshComponent;

UCLASS()
class BLASTER_API ABlasterCharacter : public ACharacter
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
	void CrouchPressed();

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

public:
	/* Weapon */
	void SetOverlappingWeapon(AWeapon* WeaponToSet);

	/* Getters */
	bool IsInAir();
	bool IsAccelerating();
	FORCEINLINE AWeapon* GetOverlappingWeapon() const { return OverlappingWeapon; }
	bool IsWeaponEquipped();
	bool IsAiming();
};
