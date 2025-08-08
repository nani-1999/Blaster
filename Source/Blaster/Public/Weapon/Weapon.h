// Copyright Na9i Studio.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Weapon.generated.h"

class USkeletalMeshComponent;
class UBoxComponent;
class UWidgetComponent;

UENUM()
enum class EWeaponState : uint8 {
	EWS_Initial,
	EWS_Equipped,
	EWS_Dropped,
	EWS_MAX
};

UCLASS()
class BLASTER_API AWeapon : public AActor
{
	GENERATED_BODY()
	
public:	
	AWeapon();

	virtual void Tick(float DeltaTime) override;

	/* Property Registerer For Replication */
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

protected:
	virtual void BeginPlay() override;

	/* Mesh */
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<USkeletalMeshComponent> WeaponMesh;

	/* Area Box */
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UBoxComponent> AreaBox;
	UFUNCTION()
	void AreaBoxBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	UFUNCTION()
	void AreaBoxEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	/* Pickup Widget */
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UWidgetComponent> PickupWidget;

	/* Weapon State */
	UPROPERTY(ReplicatedUsing = OnRep_WeaponState)
	EWeaponState WeaponState;

	void UpdateWeaponBasedOnCurrentWeaponState();

	UFUNCTION()
	void OnRep_WeaponState(EWeaponState OldState) { UpdateWeaponBasedOnCurrentWeaponState(); }

public:
	/* Pickup Widget */
	void ShowPickupWidget(bool bShow);

	/* Setters */
	void SetWeaponState(EWeaponState State) { WeaponState = State; UpdateWeaponBasedOnCurrentWeaponState(); }

	/* Getters */
	FTransform GetLeftHandSocketTransform() const;
};
