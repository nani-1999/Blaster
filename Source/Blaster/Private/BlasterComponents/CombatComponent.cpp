// Copyright Na9i Studio.

#include "BlasterComponents/CombatComponent.h"
#include "GameFramework/Character.h"
#include "Weapon/Weapon.h"
#include "Engine/SkeletalMeshSocket.h"
#include "Net/UnrealNetwork.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Animation/AnimMontage.h"
#include "Kismet/GameplayStatics.h"

#include "Blaster/Nani/NaniUtility.h"
#include "DrawDebugHelpers.h"

UCombatComponent::UCombatComponent() :
	BaseWalkSpeed{ 600.f },
	AimWalkSpeed{ 300.f }
{
	PrimaryComponentTick.bCanEverTick = true;

}

void UCombatComponent::BeginPlay()
{
	Super::BeginPlay();

}

void UCombatComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// @DEBUG
	ACharacter* CompOwner = GetOwner<ACharacter>();
	if (CompOwner && CompOwner->IsLocallyControlled()) {
		FHitResult CursorTraceHit;
		FVector EndPoint;
		TraceUnderCursor(CursorTraceHit, EndPoint);
		DrawDebugSphere(GetWorld(), CursorTraceHit.bBlockingHit ? CursorTraceHit.ImpactPoint : EndPoint, 12.f, 12.f, FColor::Red);
	} 
}

void UCombatComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const {
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	/* Properties won't be replicated unless you include them here */

	DOREPLIFETIME(UCombatComponent, EquippedWeapon);
	DOREPLIFETIME(UCombatComponent, bAiming);
	DOREPLIFETIME(UCombatComponent, bFiring);
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

//
//============================================ Montage ============================================
//
void UCombatComponent::PlayCharacterFireMontage() {
	if (EquippedWeapon == nullptr || FireMontage == nullptr) return;

	ACharacter* CompOwner = GetOwner<ACharacter>();
	if (CompOwner) {
		UAnimInstance* AnimInst = CompOwner->GetMesh()->GetAnimInstance();
		AnimInst->Montage_Play(FireMontage);

		FString WeaponType = FString("AssaultRifle");
		WeaponType += bAiming ? FString("Ironsight") : FString("Hip");
		AnimInst->Montage_JumpToSection(*WeaponType);

	}
}

//
//============================================ HitScane ============================================
//
void UCombatComponent::TraceUnderCursor(FHitResult& OutHitResult, FVector& EndPoint, float TraceLength) {

	/* this is just to get a hit target, nothing to do with weapon */

	FVector2D ViewportSize;
	if (GEngine == nullptr || GEngine->GameViewport == nullptr) return;
	GEngine->GameViewport->GetViewportSize(ViewportSize);
	FVector2D ViewportCenter = ViewportSize / 2.f;

	ACharacter* CompOwner = GetOwner<ACharacter>();
	if (CompOwner == nullptr) return;
	APlayerController* PC = nullptr;
	PC = CompOwner->GetController<APlayerController>();
	if (PC == nullptr)  return; /* this is a client failed, for simulated proxies, since they don't have controller nor viewport */

	FVector CursorWorldPosition;
	FVector CursorWorldDirection;
	bool bProjectionSuccessful = UGameplayStatics::DeprojectScreenToWorld(PC, ViewportCenter, CursorWorldPosition, CursorWorldDirection);
	if (!bProjectionSuccessful) return; /* this is a server failed, for non-owning authoritative characters, since they have controller but no viewport */

	FVector CursorEndPosition = CursorWorldPosition + TraceLength * CursorWorldDirection;

	UWorld* World = GetWorld();
	if (World) World->LineTraceSingleByChannel(OutHitResult, CursorWorldPosition, CursorEndPosition, ECollisionChannel::ECC_Visibility);

	EndPoint = CursorEndPosition;
}

//
//============================================ Fire ============================================
//
void UCombatComponent::SetFiring(bool bIsFiring) {
	if (EquippedWeapon == nullptr) return;

	FHitResult CursorTraceHit;
	FVector EndPoint;
	TraceUnderCursor(CursorTraceHit, EndPoint);

	ServerFiring(bIsFiring, bIsFiring ? CursorTraceHit.bBlockingHit ? CursorTraceHit.ImpactPoint : EndPoint : FVector::ZeroVector);
}

void UCombatComponent::ServerFiring_Implementation(bool bIsFiring, const FVector_NetQuantize HitTarget) {
	if (EquippedWeapon == nullptr) return;

	bFiring = bIsFiring;

	if (bFiring) {
		PlayCharacterFireMontage();

		EquippedWeapon->Fire(HitTarget);
	}
}

void UCombatComponent::OnRep_Firing(bool OldFiring) {
	if (bFiring) {
		/* play character fire animation */
		PlayCharacterFireMontage();

		/* this will only play weapon animation which has particle and sound 
		 * also shell eject
		 * has auth check which prevents from spawning projectile */
		EquippedWeapon->Fire(FVector::ZeroVector);
	}
}

//void UCombatComponent::SetFiring(bool bIsFiring) {
//	if (EquippedWeapon == nullptr) return;
//
//	bFiring = bIsFiring;
//	if (bFiring) {
//		/* play character fire animation */
//		PlayFireMontage();
//
//		/* play weapon fire animation, like particle and sound */
//		//FHitResult CursorTraceHit;
//		//TraceUnderCursor(CursorTraceHit, 5000.f);
//		//if (CursorTraceHit.bBlockingHit) {
//		//	EquippedWeapon->Fire(CursorTraceHit.Location);
//		//}
//	}
//}
