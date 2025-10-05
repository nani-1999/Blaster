// Copyright Na9i Studio.

#include "BlasterComponents/CombatComponent.h"
#include "GameFramework/Character.h"
#include "Weapon/Weapon.h"
#include "Engine/SkeletalMeshSocket.h"
#include "Net/UnrealNetwork.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Animation/AnimMontage.h"
#include "Kismet/GameplayStatics.h"
#include "Camera/CameraComponent.h"
#include "Interface/CombatInterface.h"
#include "Controller/BlasterPlayerController.h" 
#include "UI/BlasterUITypes.h"

//#include "UI/HUD/BlasterHUD.h"
#include "Blaster/Nani/NaniUtility.h"
#include "DrawDebugHelpers.h"

UCombatComponent::UCombatComponent() :
	BaseWalkSpeed{ 600.f },
	AimWalkSpeed{ 300.f },
	BaseFOV{ 90.f },
	InterpedFOV{ BaseFOV },
	bAllowFire{ true },
	CarriedAmmo{ 0 },
	CombatState{ ECombatState::ECS_UnOccupied }
{
	PrimaryComponentTick.bCanEverTick = true;

	/* zero initializing on all machines, but server is the real one */
	InitAllCarriedAmmo(20);

	OwnerChar = GetOwner<ACharacter>();
}

void UCombatComponent::BeginPlay()
{
	Super::BeginPlay();

	/* Reference */
	//OwnerChar = GetOwner<ACharacter>();
}

void UCombatComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (OwnerChar && BlasterPC) {
		if (EquippedWeapon) {
			TraceUnderCursor();

			/* HUD Crosshair */
			UpdateHUDCrosshair(DeltaTime);

			/* Camera
			 * Interping Camera FOV while Aiming */
			if (OwnerCharCamera) {
				InterpedFOV = FMath::FInterpTo(InterpedFOV, bAiming ? EquippedWeapon->GetAimedFOV() : BaseFOV, DeltaTime, EquippedWeapon->GetFOVInterpSpeed());
				OwnerCharCamera->SetFieldOfView(InterpedFOV);
			}
		}
		//else {
		//	/* HUD Crosshair 
		//     * removing hud crosshair if we don't have a equipped weapon */
		//	UpdateHUDCrosshair();
		//}
	}
}

void UCombatComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const {
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	/* Properties won't be replicated unless you include them here */

	DOREPLIFETIME(UCombatComponent, EquippedWeapon);
	DOREPLIFETIME(UCombatComponent, bAiming);
	DOREPLIFETIME_CONDITION(UCombatComponent, CarriedAmmo, ELifetimeCondition::COND_OwnerOnly);
	DOREPLIFETIME(UCombatComponent, CombatState);
}

//
//============================================ References ============================================
//
void UCombatComponent::SetReferences() {
	BlasterPC = OwnerChar->GetController<ABlasterPlayerController>();

	/* HUD's Overlay */
	BlasterPC->SetHUDOverlayText(EOverlayText::EOT_CarriedAmmo, GetCarriedAmmo());
	BlasterPC->SetHUDOverlayText(EOverlayText::EOT_Ammo, GetWeaponAmmo());
}

//
//============================================ Character Movement ============================================
//
void UCombatComponent::SetOrientRotationToMovement(bool bOrient) {
	if (OwnerChar) {
		OwnerChar->bUseControllerRotationYaw = !bOrient;
		OwnerChar->GetCharacterMovement()->bOrientRotationToMovement = bOrient;
	}
}

//
//============================================ Weapon ============================================
//
void UCombatComponent::EquipWeapon(AWeapon* WeaponToEquip) {
	/* Happens on Authority */
	if (WeaponToEquip == nullptr) return;

	if (EquippedWeapon) DropWeapon(); // custom

	if (OwnerChar) {
		bool bRightHandSocket = OwnerChar->GetMesh()->DoesSocketExist(FName("RightHandSocket"));
		if (bRightHandSocket) {
			WeaponToEquip->SetWeaponState(EWeaponState::EWS_Equipped);

			FAttachmentTransformRules AttachmentRules = FAttachmentTransformRules::SnapToTargetNotIncludingScale;
			bool bAttachmentSuccessful = WeaponToEquip->AttachToComponent(OwnerChar->GetMesh(), AttachmentRules, FName("RightHandSocket"));
			
			if (bAttachmentSuccessful) {
				EquippedWeapon = WeaponToEquip;
				SetOrientRotationToMovement(!EquippedWeapon/* ? false : true*/);
				EquippedWeapon->SetOwner(OwnerChar);

				/* Carried Ammo changed based on Equipped Weapon Type 
				 * find always works since it has all weapon types values */
				CarriedAmmo = AllCarriedAmmo.FindChecked(EquippedWeapon->GetWeaponType());

				/* auto reload after equipping weapon */
				Reload();
			}
			//else {
				/* change WeaponToEquip's WeaponState back to original and pretend like nothing happend */
			//}

			/* HUD's Overlay */
			if (BlasterPC) {
				BlasterPC->SetHUDOverlayText(EOverlayText::EOT_CarriedAmmo, CarriedAmmo);
				BlasterPC->SetHUDOverlayText(EOverlayText::EOT_Ammo, GetWeaponAmmo());
			}
		}
	}
}
void UCombatComponent::OnRep_EquippedWeapon(AWeapon* OldEquippedWeapon) {
	SetOrientRotationToMovement(EquippedWeapon ? false : true);

	/* HUD's Overlay */
	if (BlasterPC) {
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
	if (BlasterPC) {
		BlasterPC->SetHUDOverlayText(EOverlayText::EOT_CarriedAmmo, CarriedAmmo);
		BlasterPC->SetHUDOverlayText(EOverlayText::EOT_Ammo, GetWeaponAmmo());
	}
}

//
//============================================ Socket ============================================
//
FTransform UCombatComponent::GetWeaponGripSocket() const {
	if (EquippedWeapon) return EquippedWeapon->GetGripSocket();
	return FTransform();
}

//
//============================================ Crosshair ============================================
//
void UCombatComponent::UpdateHUDCrosshair(float DeltaTime) {
	/* Crosshair Spread Rate
	 * calculating all possible crosshair spread factors 
	 * every each factor is of a rate from 0.f to 1.f */
	/* surface velocity factor */
	FVector2D VelocityRange(0.f, BaseWalkSpeed);
	FVector2D RateRange(0.f, 1.f);
	float Velocity = OwnerChar->GetVelocity().Size2D();
	CrosshairSurfaceFactor = FMath::GetMappedRangeValueClamped(VelocityRange, RateRange, Velocity);
	/* in-air factor */
	bool bIsInAir = OwnerChar->GetCharacterMovement()->IsFalling();
	CrosshairInAirFactor = FMath::FInterpTo(CrosshairInAirFactor, bIsInAir ? 1.f : 0.f, DeltaTime, 10.f);
	/* aiming factor */
	CrosshairAimFactor = FMath::FInterpTo(CrosshairAimFactor, bAiming ? 0.f : 1.f, DeltaTime, 20.f);

	float SumOfAllFactors = CrosshairSurfaceFactor + CrosshairInAirFactor + CrosshairAimFactor;
	SumOfAllFactors *= 0.5f; /* halfing since spread is extreme */

	/* Crosshair Color  */
	FLinearColor CrosshairColor;
	ICombatInterface* CombatInterface = Cast<ICombatInterface>(CursorHitResult.GetActor());
	CrosshairColor = CombatInterface ? FLinearColor::Red : FLinearColor::White;

	BlasterPC->UpdateHUDCrosshair(EquippedWeapon->GetCrosshair(), SumOfAllFactors, CrosshairColor);
}

//
//============================================ Walk Speed ============================================
//
void UCombatComponent::SetWalkSpeed(float WalkSpeedToSet) {
	//ACharacter* CompOwner = GetOwner<ACharacter>();
	if (OwnerChar) OwnerChar->GetCharacterMovement()->MaxWalkSpeed = WalkSpeedToSet;
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
void UCombatComponent::PlayCharacterMontage(UAnimMontage* MontageToPlay, FName SectionName) {
	if (MontageToPlay && OwnerChar) {
		UAnimInstance* AnimInst = OwnerChar->GetMesh()->GetAnimInstance();
		AnimInst->Montage_Play(MontageToPlay);
		AnimInst->Montage_JumpToSection(SectionName);
	}
}

//
//============================================ HitScane ============================================
//
void UCombatComponent::TraceUnderCursor(float TraceLength, bool bOffset) {
	FVector2D ViewportSize;
	if (GEngine == nullptr || GEngine->GameViewport == nullptr) return;
	GEngine->GameViewport->GetViewportSize(ViewportSize);
	FVector2D ViewportCenter = ViewportSize / 2.f;

	/* Screen To World */
	FVector CursorWorldPosition;
	FVector CursorWorldDirection;
	bool bProjectionSuccessful = UGameplayStatics::DeprojectScreenToWorld(BlasterPC, ViewportCenter, CursorWorldPosition, CursorWorldDirection);
	if (!bProjectionSuccessful) return; /* this is a server failed, for non-owning authoritative characters, since they have controller but no viewport */
	
	/* since there are issuses like blocking itself or actors behind, so we need to offset some distance towards */
	float OffsetDistance = (CursorWorldPosition - OwnerChar->GetActorLocation()).Size() + 100.f;  /* here we are offsetting distance from screen to character location + 100.f more */

	/* Start Point */
	FVector CursorStartPosition = CursorWorldPosition + OffsetDistance * CursorWorldDirection;
	/* End Point */
	CursorEndPosition = CursorWorldPosition + TraceLength * CursorWorldDirection;

	/* Line Trace */
	GetWorld()->LineTraceSingleByChannel(CursorHitResult, bOffset ? CursorStartPosition : CursorWorldPosition, CursorEndPosition, ECollisionChannel::ECC_Visibility);
}

//
//============================================ Fire ============================================
//
void UCombatComponent::SetFiring(bool bPressed) {
	bFirePressed = bPressed;

	if (EquippedWeapon && bFirePressed) {
		if (EquippedWeapon->GetAmmo() > 0) {
			FireWeapon();
		}
		else {
			/* we only kek when bFirePressed
			 * we don't brrrrrrrrrr and kek at end 
			 * remember there will be a slight delay, since we server and from there multicasting */
			ServerFireEmpty();
			Reload();
		}
	}
}
void UCombatComponent::FireWeapon() {
	if (EquippedWeapon == nullptr || !bAllowFire || CombatState != ECombatState::ECS_UnOccupied) return;

	bAllowFire = false;

	/* only server needs to know about hittarget,
	 * local client or non owning client does not need to know hittarget or we bother sending hittarget, so ServerFIre
	 * this is to reduce bandwidth
	 * we just locally sends hittarget to server and server send nothing to all clients
	 * !note this is not always the case for some type of weapons */
	ServerFire(CursorHitResult.bBlockingHit ? CursorHitResult.ImpactPoint : CursorEndPosition);

	/* doing Timer Locally */
	GetWorld()->GetTimerManager().SetTimer(AllowFireTimerHandle, this, &UCombatComponent::AllowFire, EquippedWeapon->GetFireRate());
}
void UCombatComponent::AllowFire() {
	bAllowFire = true;

	/* checking to see if we still pressed fire button and also is weapon automatic */
	//if (bFirePressed && EquippedWeapon->IsAutomatic() && (EquippedWeapon->GetAmmo() > 0)) FireWeapon();

	/* use this instead if you want to brrrrrrr and kek at the end */
	if (EquippedWeapon && bFirePressed && EquippedWeapon->IsAutomatic()) {
		if (EquippedWeapon->GetAmmo() > 0) {
			FireWeapon();
		}
		else {
			ServerFireEmpty();
			Reload();
		}
	}
}

void UCombatComponent::ServerFire_Implementation(const FVector_NetQuantize HitTarget) {
	if (EquippedWeapon == nullptr || EquippedWeapon->GetAmmo() < 1 || CombatState != ECombatState::ECS_UnOccupied) return;
	
	/* ammo check on server too, to prevent cheating */
	//if (EquippedWeapon->GetAmmo() <= 0) return;

	/* firing bullet only on server */
	EquippedWeapon->FireBullet(HitTarget);

	/* multicast rpc must be called only by server to work
	 * multicast rpc, invoked on the server itself and all the clients that had a replicated copy of that actor
	 * unlike client rpc which only invokes on owning client of that actor, if the actor doesn't have a owning client (owner) it doesn't invoke at all */
	MulticastFire();
}
void UCombatComponent::MulticastFire_Implementation() {
	if (EquippedWeapon == nullptr) return;

	FString WeaponTypeStr = EWeaponTypeStr::ToString(EquippedWeapon->GetWeaponType());
	WeaponTypeStr += bAiming ? FString("Ironsight") : FString("Hip");
	PlayCharacterMontage(FireMontage, *WeaponTypeStr);

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
void UCombatComponent::InitAllCarriedAmmo(int InitVal) {
	/* auto sets all carried ammo of all weapon types */
	for (int i = 0; i < (int)EWeaponType::EWT_MAX; i++) {
		AllCarriedAmmo.Add((EWeaponType)i, InitVal);
	}
}
//int32 UCombatComponent::GetWeaponAmmoCapacity() const {
//	return EquippedWeapon == nullptr ? 0 : EquippedWeapon->GetAmmoCapacity();
//}
int32 UCombatComponent::GetWeaponAmmo() const {
	return EquippedWeapon ? EquippedWeapon->GetAmmo() : 0;
}

void UCombatComponent::OnRep_CarriedAmmo() {
	if (BlasterPC) BlasterPC->SetHUDOverlayText(EOverlayText::EOT_CarriedAmmo, CarriedAmmo);
}

//
//============================================ Combat ============================================
//
void UCombatComponent::OnRep_CombatState(ECombatState OldCombatState) {

	switch (CombatState) {
		case ECombatState::ECS_Reloading :
			if (EquippedWeapon == nullptr) return;
			FString WeaponTypeStr = EWeaponTypeStr::ToString(EquippedWeapon->GetWeaponType());
			//WeaponTypeStr += bAiming ? FString("Ironsight") : FString("Hip");
			PlayCharacterMontage(ReloadMontage, *WeaponTypeStr);
			break;
	}

	/* Local */
	if (BlasterPC) {
		switch (OldCombatState) {
			case ECombatState::ECS_Reloading :
			/* means reload is just finished */
			if (bFirePressed && bAllowFire && CombatState == ECombatState::ECS_UnOccupied) FireWeapon();
			break;
		}
	}
}

//
//============================================ Reload ============================================
//
void UCombatComponent::Reload() {
	if (EquippedWeapon == nullptr || CombatState != ECombatState::ECS_UnOccupied) return;

	if (CarriedAmmo > 0 && EquippedWeapon->GetAmmo() < EquippedWeapon->GetAmmoCapacity()) {
		ServerReload();
	}
	else {
		/* Reload Empty */
		//ServerReloadEmpty();
		NANI_LOG(Warning, "No Ammo To Reload | Reloading Empty");
	}
}

void UCombatComponent::ServerReload_Implementation() {
	/* Happens on Authority */

	if (EquippedWeapon == nullptr || CarriedAmmo < 1 || CombatState != ECombatState::ECS_UnOccupied) return;

	CombatState = ECombatState::ECS_Reloading;

	GetWorld()->GetTimerManager().SetTimer(ReloadTimer, this, &UCombatComponent::ReloadFinished, EquippedWeapon->GetReloadTime());

	/* playing character reload animation */
	FString WeaponTypeStr = EWeaponTypeStr::ToString(EquippedWeapon->GetWeaponType());
	//WeaponTypeStr += bAiming ? FString("Ironsight") : FString("Hip");
	PlayCharacterMontage(ReloadMontage, *WeaponTypeStr);
}
void UCombatComponent::ReloadFinished() {
	/* Happens on AUthority */

	//if (EquippedWeapon == nullptr) {
	//	CombatState = ECombatState::ECS_UnOccupied;
	//	return;
	//}

	if (EquippedWeapon && CarriedAmmo > 0) {
		/* adding ammo to weapon */
		int32& CarriedAmmoRef = AllCarriedAmmo.FindChecked(EquippedWeapon->GetWeaponType()); /* this shiz always works since we have ammo of all weapon types */
		int32 AmmoNeedToReload = EquippedWeapon->GetAmmoCapacity() - EquippedWeapon->GetAmmo();
		if (CarriedAmmoRef > AmmoNeedToReload) {
			/* carried ammo is more than ammo we need, so we add the ammo we need */
			EquippedWeapon->AddAmmo(AmmoNeedToReload);
			/* and we sub the ammo, that we added to weapon */
			CarriedAmmoRef -= AmmoNeedToReload;
		}
		else {
			EquippedWeapon->AddAmmo(CarriedAmmoRef);
			CarriedAmmoRef = 0;
		}

		CarriedAmmo = CarriedAmmoRef;

		/* HUD's Overlay */
		if (BlasterPC) {
			BlasterPC->SetHUDOverlayText(EOverlayText::EOT_CarriedAmmo, CarriedAmmo);
		}
	}

	/* in the end we back to unoccupied state 
	 * might be check for if previous reload state */
	CombatState = ECombatState::ECS_UnOccupied;
}

//void UCombatComponent::ServerReloadEmpty_Implementation() {}