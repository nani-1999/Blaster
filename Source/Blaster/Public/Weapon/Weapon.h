// Copyright Na9i Studio.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Weapon.generated.h"

class USkeletalMeshComponent;
class UBoxComponent;
class UWidgetComponent;
class UAnimationAsset;
class ACasing;

UENUM()
enum class EWeaponState : uint8 {
	EWS_Initial,
	EWS_Equipped,
	EWS_Dropped,
	EWS_MAX
};

UCLASS(Abstract)
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
	/* Animation */
	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<UAnimationAsset> FireAnimation;

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

	void UpdateWeaponState();

	UFUNCTION()
	void OnRep_WeaponState(EWeaponState OldState) { UpdateWeaponState(); }

	/* Casing */
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<ACasing> CasingClass;

	/* Field Of View */
	UPROPERTY(EditDefaultsOnly)
	float AimedFOV;
	UPROPERTY(EditDefaultsOnly)
	float FOVInterpSpeed;

	/* Fire */
	UPROPERTY(EditDefaultsOnly)
	float FireRate;
	UPROPERTY(EditDefaultsOnly)
	bool bIsAutomatic;

public:
	/* Pickup Widget */
	void ShowPickupWidget(bool bShow);

	/* Weapon State */
	void SetWeaponState(EWeaponState State) { WeaponState = State; UpdateWeaponState(); }

	/* Socket */
	FTransform GetLeftHandSocketTransform() const;

	/* Fire Bullet */
	virtual void FireBullet(const FVector& HitTarget) { }

	/* Animation */
	void PlayFireAnimation();

	/* Crosshair */
	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<UTexture2D> CrosshairCenter;
	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<UTexture2D> CrosshairTop;
	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<UTexture2D> CrosshairRight;
	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<UTexture2D> CrosshairBottom;
	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<UTexture2D> CrosshairLeft;

	/* Field Of View */
	FORCEINLINE float GetAimedFOV() const { return AimedFOV; }
	FORCEINLINE float GetFOVInterpSpeed() const { return FOVInterpSpeed; }

	/* Fire */
	FORCEINLINE float GetFireRate() const { return FireRate; }
	FORCEINLINE bool IsAutomatic() const { return bIsAutomatic; }
};
