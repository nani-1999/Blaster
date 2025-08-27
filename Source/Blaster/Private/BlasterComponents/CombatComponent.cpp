// Copyright Na9i Studio.

#include "BlasterComponents/CombatComponent.h"
#include "GameFramework/Character.h"
#include "Weapon/Weapon.h"
#include "Engine/SkeletalMeshSocket.h"
#include "Net/UnrealNetwork.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Animation/AnimMontage.h"
#include "Kismet/GameplayStatics.h"
#include "UI/HUD/BlasterHUD.h"
#include "Camera/CameraComponent.h"

#include "Blaster/Nani/NaniUtility.h"
#include "DrawDebugHelpers.h"

UCombatComponent::UCombatComponent() :
	BaseWalkSpeed{ 600.f },
	AimWalkSpeed{ 300.f },
	BaseFOV{ 90.f },
	InterpedFOV{ BaseFOV }
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
	//ACharacter* CompOwner = GetOwner<ACharacter>();
	//if (CompOwner && CompOwner->IsLocallyControlled()) {
	//	FHitResult CursorTraceHit;
	//	FVector EndPoint;
	//	TraceUnderCursor(CursorTraceHit, EndPoint);
	//	DrawDebugSphere(GetWorld(), CursorTraceHit.bBlockingHit ? CursorTraceHit.ImpactPoint : EndPoint, 12.f, 12.f, FColor::Red);
	//}

	/* Locally Controlled Tick */
	ACharacter* CompOwner = GetOwner<ACharacter>();

	if (CompOwner && CompOwner->IsLocallyControlled()) {

		/* generalizing some variables that are used in multiple function bodies */
		bool bIsInAir = CompOwner->GetCharacterMovement()->IsFalling();

		/* equipped specific */
		if (EquippedWeapon) {
			/* Updating HUD Crosshair */
			UpdateHUDCrosshair(CompOwner, bIsInAir, DeltaTime);
			/* Zoom While Aiming */
			if (CompOwnerCamera) {
				NANI_LOG(Warning, "FielfOfView: %f", CompOwnerCamera->FieldOfView);
				InterpedFOV = FMath::FInterpTo(InterpedFOV, bAiming ? EquippedWeapon->GetAimedFOV() : BaseFOV, DeltaTime, EquippedWeapon->GetFOVInterpSpeed());
				CompOwnerCamera->SetFieldOfView(InterpedFOV);
			} 
		}
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

//
//============================================ Socket ============================================
//
FTransform UCombatComponent::GetWeaponLeftHandSocketTransform() const {
	if (EquippedWeapon) return EquippedWeapon->GetLeftHandSocketTransform();
	return FTransform();
}

//
//============================================ Crosshair ============================================
//
void UCombatComponent::UpdateHUDCrosshair(ACharacter* CompOwner, bool bIsInAir, float DeltaTime) {
	//NANI_LOG(Warning, "UpdateHUDCrosshair");

	APlayerController* CompOwnerCtrl = CompOwner->GetController<APlayerController>();
	if (CompOwnerCtrl == nullptr) return;
	ABlasterHUD* BlasterHUD = CompOwnerCtrl->GetHUD<ABlasterHUD>();
	if (BlasterHUD == nullptr) return;

	FCrosshairTextures Crosshair;
	Crosshair.Center = EquippedWeapon->CrosshairCenter;
	Crosshair.Top = EquippedWeapon->CrosshairTop;
	Crosshair.Right = EquippedWeapon->CrosshairRight;
	Crosshair.Bottom = EquippedWeapon->CrosshairBottom;
	Crosshair.Left = EquippedWeapon->CrosshairLeft;

	/* calculating all possible crosshair spread factors 
	 * every each factor is of a rate from 0.f to 1.f */

	/* surface velocity factor */
	FVector2D VelocityRange(0.f, BaseWalkSpeed);
	FVector2D RateRange(0.f, 1.f);
	float Velocity = CompOwner->GetVelocity().Size2D();
	CrosshairSurfaceFactor = FMath::GetMappedRangeValueClamped(VelocityRange, RateRange, Velocity);
	/* in-air factor */
	CrosshairInAirFactor = FMath::FInterpTo(CrosshairInAirFactor, bIsInAir ? 1.f : 0.f, DeltaTime, 10.f);
	/* aiming factor */
	CrosshairAimFactor = FMath::FInterpTo(CrosshairAimFactor, bAiming ? 0.f : 1.f, DeltaTime, 20.f);

	float SumOfAllFactors = CrosshairSurfaceFactor + CrosshairInAirFactor + CrosshairAimFactor;
	BlasterHUD->UpdateCrosshair(Crosshair, SumOfAllFactors);
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

		/* play weapon animation which has particle and sound 
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
