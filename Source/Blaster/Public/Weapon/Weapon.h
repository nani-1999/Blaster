// Copyright Na9i Studio.
/* equiped, reload sounds are played in character's respective animations
 * impact particle and sounds are multicast played by weapon itself */

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "WeaponTypes.h"
#include "UI/BlasterUITypes.h"
#include "Weapon.generated.h"

class USkeletalMeshComponent;
class UBoxComponent;
class UWidgetComponent;
class UAnimationAsset;
class ACasing;
class ABlasterPlayerController;
class USoundCue;

UENUM()
enum class EWeaponState : uint8 {
	EWS_Initial,
	EWS_Equipped, /* Owned, and currently drawn */
	EWS_Holstered, /* Owned, but not drawn */
	EWS_Dropped, /* Not Owned */
	EWS_MAX
};

UCLASS(Abstract)
class BLASTER_API AWeapon : public AActor
{
	GENERATED_BODY()
	
public:	
	AWeapon();

	/* Property Registerer For Replication */
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

protected:
	virtual void BeginPlay() override;

	/* Mesh */
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<USkeletalMeshComponent> WeaponMesh;
	/* Animation */
	UPROPERTY(EditDefaultsOnly, Category = "UserClass | Weapon")
	TObjectPtr<UAnimationAsset> FireAnimation;

	/* Sound */
	UPROPERTY(EditDefaultsOnly, Category = "UserClass | Weapon")
	TObjectPtr<USoundCue> FireEmptySound;

	/* Area Box */
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UBoxComponent> AreaBox;
	UFUNCTION()
	void AreaBoxBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	UFUNCTION()
	void AreaBoxEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	/* Crosshair */
	UPROPERTY(EditDefaultsOnly, Category = "UserClass | Weapon")
	FCrosshairTextures Crosshair;

	/* Pickup Widget */
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UWidgetComponent> PickupWidget;

	/* Weapon State */
	UPROPERTY(ReplicatedUsing = OnRep_WeaponState)
	EWeaponState WeaponState;
	UFUNCTION()
	void OnRep_WeaponState(EWeaponState OldState) { UpdateWeaponState(); }

	void UpdateWeaponState();

	/* Casing */
	UPROPERTY(EditDefaultsOnly, Category = "UserClass | Weapon")
	TSubclassOf<ACasing> CasingClass;

	/* Aim FOV */
	UPROPERTY(EditDefaultsOnly, Category = "UserClass | Weapon")
	bool bCanAim;
	UPROPERTY(EditDefaultsOnly, Category = "UserClass | Weapon")
	float AimedFOV;
	UPROPERTY(EditDefaultsOnly, Category = "UserClass | Weapon")
	float FOVInterpSpeed;

	/* Fire Bullet */
	UPROPERTY(EditDefaultsOnly, Category = "UserClass | Weapon")
	float FireRate;
	UPROPERTY(EditDefaultsOnly, Category = "UserClass | Weapon")
	bool bIsAutomatic;

	/* Ammo */
	UPROPERTY(EditDefaultsOnly, Category = "UserClass | Weapon")
	int32 AmmoCapacity;
	UPROPERTY(ReplicatedUsing = OnRep_Ammo, EditDefaultsOnly, Category = "UserClass | Weapon")
	int32 Ammo;
	UFUNCTION()
	virtual void OnRep_Ammo();

	/* Weapon Type */
	UPROPERTY(EditDefaultsOnly, Category = "UserClass | Weapon")
	EWeaponType WeaponType;

	/* Reload */
	UPROPERTY(EditDefaultsOnly, Category = "UserClass | Weapon")
	float ReloadTime;

	/* References */
	ABlasterPlayerController* BlasterPC;
	void DetermineOwnerLocal(AActor* NetLocal);
	virtual void OnRep_Owner() override;

public:
	/* Pickup Widget */
	void ShowPickupWidget(bool bShow);

	/* Weapon State */
	void SetWeaponState(EWeaponState State) { WeaponState = State; UpdateWeaponState(); }

	/* Socket */
	FTransform GetGripSocket() const;

	/* Fire Bullet */
	virtual void FireBullet(const FVector& HitTarget);

	/* Animation */
	void PlayFireAnimation();
	void PlayFireEmpty();

	/* Crosshair */
	FCrosshairTextures& GetCrosshair() { return Crosshair; }

	/* Aim FOV */
	FORCEINLINE bool CanAim() const { return bCanAim; }
	FORCEINLINE float GetAimedFOV() const { return AimedFOV; }
	FORCEINLINE float GetFOVInterpSpeed() const { return FOVInterpSpeed; }

	/* Fire */
	FORCEINLINE float GetFireRate() const { return FireRate; }
	FORCEINLINE bool IsAutomatic() const { return bIsAutomatic; }

	/* Ammo */
	FORCEINLINE int32 GetAmmoCapacity() const { return AmmoCapacity; }
	FORCEINLINE int32 GetAmmo() const { return Ammo; }
	void AddAmmo(int32 AddAmount);

	/* Weapon Type */
	FORCEINLINE EWeaponType GetWeaponType() const { return WeaponType; }

	/* Reload */
	FORCEINLINE float GetReloadTime() const { return ReloadTime; }

	/* HUD */
	virtual void SetOwner(AActor* NewOwner) override;

	/* test */
	UFUNCTION(Server, Reliable)
	void ServerTest();
};
