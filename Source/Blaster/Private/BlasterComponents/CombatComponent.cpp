// Copyright Na9i Studio.


#include "BlasterComponents/CombatComponent.h"
#include "GameFramework/Character.h"
#include "Weapon/Weapon.h"
#include "Engine/SkeletalMeshSocket.h"
#include "Net/UnrealNetwork.h"
#include "GameFramework/CharacterMovementComponent.h"

#include "Blaster/Nani/NaniUtility.h"

UCombatComponent::UCombatComponent() :
	BaseWalkSpeed{ 600.f },
	AimWalkSpeed{ 300.f }
{
	PrimaryComponentTick.bCanEverTick = false;

}

void UCombatComponent::BeginPlay()
{
	Super::BeginPlay();

}

void UCombatComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

}

void UCombatComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const {
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UCombatComponent, EquippedWeapon);
	DOREPLIFETIME(UCombatComponent, bAiming);
}

//
//============================================ Character Movement ============================================
//
void UCombatComponent::SetOrientRotationToMovement(bool bOrient) {
	ACharacter* CompOwner = GetOwner<ACharacter>();
	if (CompOwner) {
		UCharacterMovementComponent* MovementComp = CompOwner->GetCharacterMovement();
		CompOwner->bUseControllerRotationYaw = !bOrient;
		MovementComp->bOrientRotationToMovement = bOrient;
	}
}

//
//============================================ Weapon ============================================
//
void UCombatComponent::EquipWeapon(AWeapon* WeaponToEquip) {
	if (WeaponToEquip == nullptr) return;

	ACharacter* CompOwner = GetOwner<ACharacter>();
	if (CompOwner) {
		bool bRightHandSocket = CompOwner->GetMesh()->DoesSocketExist(FName("RightHandSocket"));
		if (bRightHandSocket) {
			WeaponToEquip->SetWeaponState(EWeaponState::EWS_Equipped);

			FAttachmentTransformRules AttachmentRules = FAttachmentTransformRules::SnapToTargetNotIncludingScale;
			bool bAttachmentSuccessful = WeaponToEquip->AttachToComponent(CompOwner->GetMesh(), AttachmentRules, FName("RightHandSocket"));
			
			if (bAttachmentSuccessful) {
				EquippedWeapon = WeaponToEquip;
				SetOrientRotationToMovement(EquippedWeapon ? false : true);
				EquippedWeapon->SetOwner(CompOwner);
			}
			//else {
				/* change WeaponToEquip's WeaponState back to original and pretend like nothing happend */
			//}
		}
	}
}
void UCombatComponent::OnRep_EquippedWeapon(AWeapon* OldEquippedWeapon) {
	SetOrientRotationToMovement(EquippedWeapon ? false : true);
}

FTransform UCombatComponent::GetWeaponLeftHandSocketTransform() const {
	if (EquippedWeapon) return EquippedWeapon->GetLeftHandSocketTransform();
	return FTransform();
}

//
//============================================ Walk Speed ============================================
//
void UCombatComponent::SetWalkSpeed(float WalkSpeedToSet) {
	ACharacter* CompOwner = GetOwner<ACharacter>();
	if (CompOwner) {
		UCharacterMovementComponent* MovementComp = CompOwner->GetCharacterMovement();
		MovementComp->MaxWalkSpeed = WalkSpeedToSet;
	}
}
//
//============================================ Aim ============================================
//
void UCombatComponent::SetAiming(bool bIsAiming) {
	if (EquippedWeapon == nullptr) return;

	bAiming = bIsAiming;
	SetWalkSpeed(bAiming ? AimWalkSpeed : BaseWalkSpeed);
}
void UCombatComponent::OnRep_Aiming(bool OldAiming) {
	SetWalkSpeed(bAiming ? AimWalkSpeed : BaseWalkSpeed);
}