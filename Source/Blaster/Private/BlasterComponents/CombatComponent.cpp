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
#include "Interface/CombatInterface.h"
#include "Controller/BlasterPlayerController.h" 

#include "Blaster/Nani/NaniUtility.h"
#include "DrawDebugHelpers.h"

UCombatComponent::UCombatComponent() :
	BaseWalkSpeed{ 600.f },
	AimWalkSpeed{ 300.f },
	BaseFOV{ 90.f },
	InterpedFOV{ BaseFOV },
	bAllowFire{ true },
	CarriedAmmo{ 0 }
{
	PrimaryComponentTick.bCanEverTick = true;

	AllCarriedAmmo.Add(EWeaponType::EWT_AssaultRifle, 69);
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
		/* equipped specific */
		if (EquippedWeapon) {

			/* generalizing some variables that are used in multiple function bodies */
			bool bIsInAir = CompOwner->GetCharacterMovement()->IsFalling();

			/* Updating HUD Crosshair */
			UpdateHUDCrosshair(CompOwner, bIsInAir, DeltaTime);

			/* Zoom While Aiming */
			if (CompOwnerCamera) {
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
	DOREPLIFETIME_CONDITION(UCombatComponent, CarriedAmmo, ELifetimeCondition::COND_OwnerOnly);
	//DOREPLIFETIME(UCombatComponent, bFiring);
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
	/* Happens on Authority */
	if (WeaponToEquip == nullptr) return;

	if (EquippedWeapon) DropWeapon(); // custom

	ACharacter* CompOwner = GetOwner<ACharacter>();
	if (CompOwner) {
		bool bRightHandSocket = CompOwner->GetMesh()->DoesSocketExist(FName("RightHandSocket"));
		if (bRightHandSocket) {
			WeaponToEquip->SetWeaponState(EWeaponState::EWS_Equipped);

			FAttachmentTransformRules AttachmentRules = FAttachmentTransformRules::SnapToTargetNotIncludingScale;
			bool bAttachmentSuccessful = WeaponToEquip->AttachToComponent(CompOwner->GetMesh(), AttachmentRules, FName("RightHandSocket"));
			
			if (bAttachmentSuccessful) {
				EquippedWeapon = WeaponToEquip;
				SetOrientRotationToMovement(!EquippedWeapon/* ? false : true*/);
				EquippedWeapon->SetOwner(CompOwner);

				/* Carried Ammo changed based on Equipped Weapon Type 
				 * find always since it has all weapon types values */
				CarriedAmmo = AllCarriedAmmo.FindChecked(EquippedWeapon->GetWeaponType());
			}
			//else {
				/* change WeaponToEquip's WeaponState back to original and pretend like nothing happend */
			//}

			/* HUD's Overlay */
			if (CompOwner->IsLocallyControlled()) {
				ABlasterPlayerController* BlasterPC = CompOwner->GetController<ABlasterPlayerController>();
				BlasterPC->SetHUDOverlayText(EOverlayText::EOT_CarriedAmmo, CarriedAmmo);
				BlasterPC->SetHUDOverlayText(EOverlayText::EOT_Ammo, GetWeaponAmmo());
			}
		}
	}
}
void UCombatComponent::OnRep_EquippedWeapon(AWeapon* OldEquippedWeapon) {
	SetOrientRotationToMovement(EquippedWeapon ? false : true);

	/* HUD's Overlay */
	ACharacter* CompOwner = GetOwner<ACharacter>();
	if (CompOwner && CompOwner->IsLocallyControlled()) {
		ABlasterPlayerController* BlasterPC = CompOwner->GetController<ABlasterPlayerController>();
		BlasterPC->SetHUDOverlayText(EOverlayText::EOT_CarriedAmmo, CarriedAmmo);
		BlasterPC->SetHUDOverlayText(EOverlayText::EOT_Ammo, GetWeaponAmmo());
	}
}
void UCombatComponent::DropWeapon() {
	/* Happens on Authority */
	if (EquippedWeapon == nullptr) return;

	/* detaching weapon */
	EquippedWeapon->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
	
	/* setting detached weapon's location to our forward */
	//ACharacter* CompOwner = GetOwner<ACharacter>();
	//if (CompOwner) {
	//	FVector ForwardLocation = CompOwner->GetActorLocation() + CompOwner->GetActorForwardVector() * 200.f;
	//	EquippedWeapon->SetActorLocation(ForwardLocation);
	//}

	/* must also set weapon's owner to null */
	EquippedWeapon->SetOwner(nullptr);

	/* setting weapon state and nulling out our weapon ptr */
	EquippedWeapon->SetWeaponState(EWeaponState::EWS_Dropped);
	EquippedWeapon = nullptr;

	/* no equipped weapon, no carried ammo */
	CarriedAmmo = 0;

	/* HUD's Overlay */
	ACharacter* CompOwner = GetOwner<ACharacter>();
	if (CompOwner && CompOwner->IsLocallyControlled()) {
		ABlasterPlayerController* BlasterPC = CompOwner->GetController<ABlasterPlayerController>();
		BlasterPC->SetHUDOverlayText(EOverlayText::EOT_CarriedAmmo, CarriedAmmo);
		BlasterPC->SetHUDOverlayText(EOverlayText::EOT_Ammo, GetWeaponAmmo());
	}
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

	/* Crosshair Textures */
	FCrosshairTextures Crosshair;
	Crosshair.Center = EquippedWeapon->CrosshairCenter;
	Crosshair.Top = EquippedWeapon->CrosshairTop;
	Crosshair.Right = EquippedWeapon->CrosshairRight;
	Crosshair.Bottom = EquippedWeapon->CrosshairBottom;
	Crosshair.Left = EquippedWeapon->CrosshairLeft;

	/* Crosshair Spread Rate
	 * calculating all possible crosshair spread factors 
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

	/* Crosshair Color  */
	FLinearColor CrosshairColor;
	FHitResult CursorTraceHit;
	FVector CursorEndPoint; /* satisfy, just gets the end point if nothing hits */
	TraceUnderCursor(CursorTraceHit, CursorEndPoint);
	ICombatInterface* CombatInterface = Cast<ICombatInterface>(CursorTraceHit.GetActor());
	CrosshairColor = CombatInterface ? FLinearColor::Red : FLinearColor::White;

	BlasterHUD->UpdateCrosshair(Crosshair, SumOfAllFactors, CrosshairColor);
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
void UCombatComponent::TraceUnderCursor(FHitResult& OutHitResult, FVector& CursorEndPosition, float TraceLength, bool bOffset) {

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

	/* Screen To World */
	FVector CursorWorldPosition;
	FVector CursorWorldDirection;
	bool bProjectionSuccessful = UGameplayStatics::DeprojectScreenToWorld(PC, ViewportCenter, CursorWorldPosition, CursorWorldDirection);
	if (!bProjectionSuccessful) return; /* this is a server failed, for non-owning authoritative characters, since they have controller but no viewport */
	/* Start Point
	 * since there are issuses like blocking itself and actors behind, so we need to offset some distance towards */
	float OffsetDistance = (CursorWorldPosition - CompOwner->GetActorLocation()).Size() + 100.f;  /* here we are offsetting distance from screen to character location + 100.f more */
	FVector CursorStartPosition = CursorWorldPosition + OffsetDistance * CursorWorldDirection;
	/* End Point */
	CursorEndPosition = CursorWorldPosition + TraceLength * CursorWorldDirection;

	/* Line Trace */
	if (UWorld* World = GetWorld()) World->LineTraceSingleByChannel(OutHitResult, bOffset ? CursorStartPosition : CursorWorldPosition, CursorEndPosition, ECollisionChannel::ECC_Visibility);
}

//
//============================================ Fire ============================================
//
void UCombatComponent::SetFiring(bool bPressed) {
	if (EquippedWeapon == nullptr) return;

	bFirePressed = bPressed; /* FireButtonPressed */

	if (bFirePressed && bAllowFire) {
		if (EquippedWeapon->GetAmmo() > 0) {
			FireWeapon();
		}
		else {
			/* we only kek when bFirePressed
			 * we don't brrrrrrrrrr and kek at end 
			 * remember there will be a slight delay, since we server and from there multicasting */
			ServerFireEmpty();
		}
	}
}
void UCombatComponent::FireWeapon() {
	bAllowFire = false;

	/* Local */
	FHitResult CursorTraceHit;
	FVector CursorEndPoint;
	TraceUnderCursor(CursorTraceHit, CursorEndPoint);

	/* only server needs to know about hittarget,
	 * local client or non owning client does not need to know hittarget or we bother sending hittarget
	 * this is to reduce bandwidth
	 * we just locally sends hittarget to server and server send nothing to all clients
	 * !note this is not always the case for some type of weapons */
	ServerFire(CursorTraceHit.bBlockingHit ? CursorTraceHit.ImpactPoint : CursorEndPoint);

	/* doing Timer Locally */
	GetWorld()->GetTimerManager().SetTimer(AllowFireTimerHandle, this, &UCombatComponent::AllowFire, EquippedWeapon->GetFireRate());
}
void UCombatComponent::AllowFire() {
	bAllowFire = true;

	/* checking to see if we still pressed fire button and also is weapon automatic */
	if (bFirePressed && EquippedWeapon->IsAutomatic() && (EquippedWeapon->GetAmmo() > 0)) FireWeapon();

	/* use this instead if you want to brrrrrrr and kek at the end */
	//if (bFirePressed && EquippedWeapon->IsAutomatic()) {
	//	if (EquippedWeapon->GetAmmo() > 0) {
	//		FireWeapon();
	//	}
	//	else {
	//		ServerFireEmpty();
	//	}
	//}
}

void UCombatComponent::ServerFire_Implementation(const FVector_NetQuantize HitTarget) {
	if (EquippedWeapon == nullptr) return;
	
	/* ammo check on server too, to prevent cheating */
	if (EquippedWeapon->GetAmmo() <= 0) return;

	/* firing bullet only on server */
	EquippedWeapon->FireBullet(HitTarget);

	/* multicast rpc must be called only by server to work
	 * multicast rpc, invoked on the server itself and all the clients that had a replicated copy of that actor
	 * unlike client rpc which only invokes on owning client of that actor, if the actor doesn't have a owning client (owner) it doesn't invoke at all */
	MulticastFire();
}
void UCombatComponent::MulticastFire_Implementation() {
	if (EquippedWeapon == nullptr) return;

	PlayCharacterFireMontage();
	EquippedWeapon->PlayFireAnimation();
}

void UCombatComponent::ServerFireEmpty_Implementation() {
	if (EquippedWeapon == nullptr) return;

	MulticastFireEmpty();
}
void UCombatComponent::MulticastFireEmpty_Implementation() {
	if (EquippedWeapon == nullptr) return;

	EquippedWeapon->PlayFireEmpty();
}

//
//============================================ Ammo ============================================
//
//int32 UCombatComponent::GetWeaponAmmoCapacity() const {
//	return EquippedWeapon == nullptr ? 0 : EquippedWeapon->GetAmmoCapacity();
//}
int32 UCombatComponent::GetWeaponAmmo() const {
	return EquippedWeapon == nullptr ? 0 : EquippedWeapon->GetAmmo();
}

void UCombatComponent::OnRep_CarriedAmmo() {
	NANI_LOG(Warning, "OnRep_CarriedAmmo");

	if (ACharacter* CompOwner = GetOwner<ACharacter>()) {
		if (ABlasterPlayerController* CompOwnerPC = CompOwner->GetController<ABlasterPlayerController>()) {
			if (CompOwnerPC->IsLocalController()) {
				CompOwnerPC->SetHUDOverlayText(EOverlayText::EOT_CarriedAmmo, CarriedAmmo);
			}
		}
	}
}
