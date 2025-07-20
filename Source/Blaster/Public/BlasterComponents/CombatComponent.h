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

	/* Combat */
	UPROPERTY(Replicated)
	TObjectPtr<AWeapon> EquippedWeapon;

public:
	void EquipWeapon(AWeapon* WeaponToEquip);

	/* Getters */
	FORCEINLINE AWeapon* GetEquippedWeapon() const { return EquippedWeapon; }
};
