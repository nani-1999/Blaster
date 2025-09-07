// Copyright Na9i Studio.


#include "Character/BlasterCharacter.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Net/UnrealNetwork.h"
#include "Weapon/Weapon.h"
#include "Components/WidgetComponent.h"
#include "UI/Widget/TextWidget.h"
#include "GameFramework/PlayerState.h"
#include "Components/CapsuleComponent.h"
#include "BlasterComponents/CombatComponent.h"
#include "Controller/BlasterPlayerController.h"
//#include "Animation/AnimMontage.h"
#include "GameMode/BlasterGameMode.h"
#include "Components/TimelineComponent.h"
#include "Kismet/GameplayStatics.h"
//#include "Particles/ParticleSystemComponent.h"
#include "Particles/ParticleSystem.h"
#include "Sound/SoundCue.h"
#include "PlayerState/BlasterPlayerState.h"

#include "Blaster/Nani/NaniUtility.h"

ABlasterCharacter::ABlasterCharacter() :
	MaxHealth{ 100.f },
	CurrentHealth{ 80.f },
	bEliminated{ false },
	ElimAnimTime{ 5.f }
{
	PrimaryActorTick.bCanEverTick = false;
	/* Network */
	NetUpdateFrequency = 66.f;
	MinNetUpdateFrequency = 33.f;

	/* Spawning 
	 * colliding something when trying to spawn */
	SpawnCollisionHandlingMethod = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

	/* Unblocking Camera over Pawn */
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);

	/* Character Skeletal Mesh */
	USkeletalMeshComponent* SKMesh = GetMesh();
	SKMesh->SetCollisionObjectType(ECollisionChannel::ECC_PhysicsBody);
	SKMesh->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore); /* including camera */
	SKMesh->SetCollisionResponseToChannel(ECollisionChannel::ECC_WorldDynamic, ECollisionResponse::ECR_Block); /* for projectile */
	SKMesh->SetCollisionResponseToChannel(ECollisionChannel::ECC_Visibility, ECollisionResponse::ECR_Block); /* for crosshair trace line */

	/* Camera Boom */
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>("CameraBoom");
	CameraBoom->SetupAttachment(GetMesh()); /* idk why mesh tho, maybe because to move boom along the animations */
	//CameraBoom->SetUsingAbsoluteRotation(true);
	CameraBoom->TargetArmLength = 350.f;
	CameraBoom->SocketOffset = FVector(0.f, 75.f, 75.f); /* offsetting cameraboom's end point to the right and up, so that the character is not on the way of crosshair */
	CameraBoom->bUsePawnControlRotation = true;

	/* Follow Camera */
	FollowCamera = CreateDefaultSubobject<UCameraComponent>("FollowCamera");
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	//FollowCamera->bUsePawnControlRotation = false;
	
	/* Combat */
	Combat = CreateDefaultSubobject<UCombatComponent>("Combat");
	Combat->SetIsReplicated(true);

	/* OrientRotationToMovement */
	bUseControllerRotationYaw = false; /* since other two axis are false by default */
	GetCharacterMovement()->bOrientRotationToMovement = true;

	/* Character Movement */
	GetCharacterMovement()->MaxWalkSpeed = Combat->GetBaseWalkSpeed();
	GetCharacterMovement()->JumpZVelocity = 1600.f;
	GetCharacterMovement()->GravityScale = 2.f;

	/* Overhead Widget */
	OverheadWidget = CreateDefaultSubobject<UWidgetComponent>("OverheadWidget");
	OverheadWidget->SetupAttachment(GetRootComponent());
	OverheadWidget->SetWidgetSpace(EWidgetSpace::Screen);
	OverheadWidget->SetVisibility(false);
	OverheadWidget->SetDrawAtDesiredSize(true);

	/* Crouch 
	 * enabling crouch
	 * also crouch is auto replicated since its handled by CharacterMovement 
	 * replication triggered by changing bWantsToCrouch, it also checking if can crouch */
	GetCharacterMovement()->NavAgentProps.bCanCrouch = true;
	GetCharacterMovement()->SetCrouchedHalfHeight(60.f); /* accessing CrouchedHalfHeight directly is depricated, using set instead */
	GetCharacterMovement()->MaxWalkSpeedCrouched = 200.f;

	/* Timeline */
	Transition = CreateDefaultSubobject<UTimelineComponent>("Transition");

	/* Elim Bot */
	//ElimParticleComp = CreateDefaultSubobject<UParticleSystemComponent>("ElimParticleComp");
	//ElimParticleComp->SetupAttachment(GetRootComponent());
	//ElimParticleComp->SetAutoActivate(false);
}

/* might do 
 * one time references, instead of casting everytime 
 * one time references for local hud */

void ABlasterCharacter::BeginPlay()
{
	Super::BeginPlay();

	//SetupOverheadWidget();

	if (HasAuthority()) {
		OnTakeAnyDamage.AddDynamic(this, &ABlasterCharacter::TakenAnyDamage);
	}

	/* must be in beginplay not in constructor, since we need to know network role 
	 * combat component needs camera for fov interping while aiming */
	if (IsLocallyControlled()) {
		Combat->SetCamera(FollowCamera);
	}
}

void ABlasterCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

//void ABlasterCharacter::PostInitializeComponents() {
//	Super::PostInitializeComponents();
//
//	if (Combat) {
//		Combat->Character = this;
//	}
//}

//
//============================================ Replication ============================================
//
void ABlasterCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const {
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION(ABlasterCharacter, OverlappingWeapon, COND_OwnerOnly); /* replicates to owning client only */
	//DOREPLIFETIME_CONDITION(ABlasterCharacter, CurrentHealth, COND_OwnerOnly);
	DOREPLIFETIME(ABlasterCharacter, CurrentHealth);
}

void ABlasterCharacter::PossessedBy(AController* NewController) {
	Super::PossessedBy(NewController);

	NANI_LOG(Warning, "PossessedBy");

	SetupOverheadWidget();

	if (IsLocallyControlled()) {
		/* HUD's Overlay
		 * this is where the character is completely valid and we can access stat values to set on HUD
		 * using Controller as a mediator to setup and set stats on hud's overlay */
		if (ABlasterPlayerController* BlasterPC = Cast<ABlasterPlayerController>(NewController)) {
			/* setting up hud's overlay */
			BlasterPC->SetupHUDOverlay();
			/* and initializing its values */
			BlasterPC->SetHUDOverlayHealth(CurrentHealth, MaxHealth);
			if (ABlasterPlayerState* BlasterPS = GetPlayerState<ABlasterPlayerState>()) {
				BlasterPC->SetHUDOverlayScore(BlasterPS->GetScore());
				BlasterPC->SetHUDOverlayDefeats(BlasterPS->GetDefeats());
			}
		}
	}
}
void ABlasterCharacter::OnRep_PlayerState() {
	Super::OnRep_PlayerState();

	NANI_LOG(Warning, "OnRep_PlayerState");

	SetupOverheadWidget();

	if (IsLocallyControlled()) {
		/* HUD's Overlay
		 * this is where the character is completely valid and we can access stat values to set on HUD
		 * using Controller as a mediator to setup and set stats on hud's overlay
		 * usually we make widget controller to manage widgets */
		if (ABlasterPlayerController* BlasterPC = GetController<ABlasterPlayerController>()) {
			/* setting up hud's overlay */
			BlasterPC->SetupHUDOverlay();
			/* and initializing its values */
			BlasterPC->SetHUDOverlayHealth(CurrentHealth, MaxHealth);
			if (ABlasterPlayerState* BlasterPS = GetPlayerState<ABlasterPlayerState>()) {
				BlasterPC->SetHUDOverlayScore(BlasterPS->GetScore());
				BlasterPC->SetHUDOverlayDefeats(BlasterPS->GetDefeats());
			}
		}
	}
}

//
//============================================ Overhead Widget ============================================
//
void ABlasterCharacter::SetupOverheadWidget() {
	/* some times in early stages widgetcomponent's widget is not intialized(beginplay called)
	 * we have to initialize if not, no worries because it has checks if widget is already created or not
	 * peek for more, has weird call of InitWidget() on its BeginPlay() */
	OverheadWidget->InitWidget();
	UTextWidget* TextWidget = Cast<UTextWidget>(OverheadWidget->GetUserWidgetObject());
	if (TextWidget) {
		FString NetRoleStr = GetNetRoleStr<FString>(GetLocalRole());
		FString PlayerName = GetPlayerState<APlayerState>()->GetPlayerName();
		FString TextStr = NetRoleStr + "\n" + PlayerName;

		TextWidget->SetText(FText::FromString(TextStr), 20);
		TextWidget->SetVisibility(ESlateVisibility::HitTestInvisible);
		OverheadWidget->SetVisibility(true);
	}
}

//
//============================================ Weapon ============================================
//
void ABlasterCharacter::SetOverlappingWeapon(AWeapon* WeaponToSet) { 
	/* Before Set its Old */
	if (OverlappingWeapon && IsLocallyControlled()) {
		OverlappingWeapon->ShowPickupWidget(false);
	}

	OverlappingWeapon = WeaponToSet; 

	/* After Set its New */
	if (OverlappingWeapon && IsLocallyControlled()) {
		OverlappingWeapon->ShowPickupWidget(true);
	}
}
void ABlasterCharacter::OnRep_OverlappingWeapon(AWeapon* OldWeapon) const {
	if (OldWeapon) {
		OldWeapon->ShowPickupWidget(false);
	}
	if (OverlappingWeapon) {
		OverlappingWeapon->ShowPickupWidget(true);
	}
}

FTransform ABlasterCharacter::GetWeaponLeftHandSocketTransform() const {
	if (Combat) return Combat->GetWeaponLeftHandSocketTransform();
	return FTransform();
}

//
//============================================ Input ============================================
//
void ABlasterCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAxis("MoveForward", this, &ABlasterCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &ABlasterCharacter::MoveRight);
	PlayerInputComponent->BindAxis("LookUp", this, &ABlasterCharacter::LookUp);
	PlayerInputComponent->BindAxis("Turn", this, &ABlasterCharacter::Turn);

	PlayerInputComponent->BindAction("Jump", EInputEvent::IE_Pressed, this, &ABlasterCharacter::Jump);
	PlayerInputComponent->BindAction("Equip", EInputEvent::IE_Pressed, this, &ABlasterCharacter::EquipPressed);
	PlayerInputComponent->BindAction("Crouch", EInputEvent::IE_Pressed, this, &ABlasterCharacter::CrouchPressed);
	PlayerInputComponent->BindAction("Aim", EInputEvent::IE_Pressed, this, &ABlasterCharacter::AimPressed);
	PlayerInputComponent->BindAction("Aim", EInputEvent::IE_Released, this, &ABlasterCharacter::AimReleased);
	PlayerInputComponent->BindAction("Fire", EInputEvent::IE_Pressed, this, &ABlasterCharacter::FirePressed);
	PlayerInputComponent->BindAction("Fire", EInputEvent::IE_Released, this, &ABlasterCharacter::FireReleased);

	PlayerInputComponent->BindAction("Test", EInputEvent::IE_Pressed, this, &ABlasterCharacter::TestPressed);
}

void ABlasterCharacter::TestPressed() {
	ABlasterPlayerState* BlasterPS = GetPlayerState<ABlasterPlayerState>();
	if (BlasterPS) BlasterPS->AddScore(1.f);

	NANI_LOG(Warning, "TestPressed");
	ServerTestPressed();
}
void ABlasterCharacter::ServerTestPressed_Implementation() {

	NANI_LOG(Warning, "ServerTestPressed");
	//CurrentHealth += 5.f;
	//if (IsLocallyControlled()) {
	//	/* always do hud updates on locally controlled clients only */
	//	if (ABlasterPlayerController* BlasterPC = GetController<ABlasterPlayerController>()) {
	//		/* using player controller as a mediatory to set health on hud */
	//		BlasterPC->SetHUDOverlayHealth(CurrentHealth, MaxHealth);
	//	}
	//}
	MulticastTestPressed();
}
void ABlasterCharacter::MulticastTestPressed_Implementation() {

	NANI_LOG(Warning, "MulticastTestPressed");
}

void ABlasterCharacter::MoveForward(const float Value) {
	if (Value && Controller) {
		FRotator ControllerYaw = FRotator(0.f, GetController()->GetControlRotation().Yaw, 0.f);
		FVector ForwardVector = FRotationMatrix(ControllerYaw).GetUnitAxis(EAxis::X);
		AddMovementInput(ForwardVector, Value);
	}
}
void ABlasterCharacter::MoveRight(const float Value) {
	if (Value && Controller) {
		FRotator ControllerYaw = FRotator(0.f, GetController()->GetControlRotation().Yaw, 0.f);
		FVector RightVector = FRotationMatrix(ControllerYaw).GetUnitAxis(EAxis::Y);
		AddMovementInput(RightVector, Value);
	}
}
void ABlasterCharacter::LookUp(const float Value) {
	if (Value && Controller) {
		AddControllerPitchInput(Value);
	}
}
void ABlasterCharacter::Turn(const float Value) {
	if (Value && Controller) {
		AddControllerYawInput(Value);
	}
}

void ABlasterCharacter::EquipPressed() {
	/*if (HasAuthority()) {
		if (Combat) Combat->EquipWeapon(OverlappingWeapon);
	}
	else {
		ServerEquipPressed();
	}*/

	ServerEquipPressed();
}
void ABlasterCharacter::ServerEquipPressed_Implementation() {
	if (Combat) Combat->EquipWeapon(OverlappingWeapon);
}

void ABlasterCharacter::CrouchPressed() {
	if (IsInAir()) return;

	if (bIsCrouched) {
		UnCrouch();
	}
	else {
		Crouch();
	}
}

void ABlasterCharacter::Jump() {
	if (bIsCrouched) {
		UnCrouch();
	}
	else {
		ACharacter::Jump();
	}
}

void ABlasterCharacter::AimPressed() {
	/*if (HasAuthority()) {
		if (Combat) Combat->SetAiming(true);
	}
	else {
		ServerAimPressed();
	}*/

	ServerAimPressed();
}
void ABlasterCharacter::ServerAimPressed_Implementation() {
	if (Combat) Combat->SetAiming(true);
}
void ABlasterCharacter::AimReleased() {
	/*if (HasAuthority()) {
		if (Combat) Combat->SetAiming(false);
	}
	else {
		ServerAimReleased();
	}*/

	ServerAimReleased();
}
void ABlasterCharacter::ServerAimReleased_Implementation() {
	if (Combat) Combat->SetAiming(false);
}

void ABlasterCharacter::FirePressed() {
	/* since RPCs are handled by Combat Component */
	if (Combat) Combat->SetFiring(true);
}
void ABlasterCharacter::FireReleased() {
	if (Combat) Combat->SetFiring(false);
}

//
//============================================ Play Montage ============================================
//
//void ABlasterCharacter::PlayHitReactMontage() {
//	if (HitReactMontage) {
//		UAnimInstance* AnimInst = GetMesh()->GetAnimInstance();
//		if (AnimInst) {
//			AnimInst->Montage_Play(HitReactMontage);
//			AnimInst->Montage_JumpToSection(FName("Front"));
//		}
//	}
//}
void ABlasterCharacter::PlayMontage(UAnimMontage* MontageToPlay, FName SectionName) {
	if (MontageToPlay) {
		UAnimInstance* AnimInst = GetMesh()->GetAnimInstance();
		if (AnimInst) {
			AnimInst->Montage_Play(MontageToPlay);
			AnimInst->Montage_JumpToSection(SectionName);
		}
	}
}


//
//============================================ Stats ============================================
//
void ABlasterCharacter::OnRep_CurrentHealth(float OldCurrentHealth) {
	/* HUD
	 * always do hud updates on locally controlled clients only
	 * using player controller as a mediatory to set health on hud */
	if (IsLocallyControlled()) {
		if (ABlasterPlayerController* BlasterPC = GetController<ABlasterPlayerController>()) {
			BlasterPC->SetHUDOverlayHealth(CurrentHealth, MaxHealth);
		}
	}

	if (CurrentHealth && CurrentHealth < OldCurrentHealth) {
		/* means health is decreased 
	     * checking currenthealth to prevent elimination montage confliction */
		PlayMontage(HitReactMontage, FName("Front"));
	}
}

//
//============================================ Damage ============================================
//
void ABlasterCharacter::TakenAnyDamage(AActor* DamagedActor, float Damage, const UDamageType* DamageType, AController* InstigatedBy, AActor* DamageCauser) {
	NANI_LOG(Warning, "%s Dealt %f Damage on %s using %s", *InstigatedBy->GetName(), Damage, *DamagedActor->GetName(), *DamageCauser->GetName());

	/* Happens on Authority */
	CurrentHealth = FMath::Clamp(CurrentHealth - Damage, 0.f, MaxHealth);

	/* HUD 
	 * always do hud updates on locally controlled clients only */
	if (IsLocallyControlled()) {
		if (ABlasterPlayerController* BlasterPC = GetController<ABlasterPlayerController>()) {
			BlasterPC->SetHUDOverlayHealth(CurrentHealth, MaxHealth);
		}
	}

	/* checking for Elimination */
	if (CurrentHealth <= 0.f) {
		if (ABlasterGameMode* BlasterGM = GetWorld()->GetAuthGameMode<ABlasterGameMode>()) {
			BlasterGM->EliminatePlayer(this, Controller, InstigatedBy);
		}
	}
	else {
		/* means health is above 0.f and we play hit react montage */
		PlayMontage(HitReactMontage, FName("Front"));
	}
}

//
//============================================ Elimination ============================================
//
void ABlasterCharacter::Eliminated() {
	/* we does equip on server, so unequip must also be on server */
	if (Combat) Combat->UnEquipWeapon();

	/* Happens on Authority */
	MulticastEliminated();

	/* this is purely for elimination animation montage to complete */
	GetWorldTimerManager().SetTimer(ElimAnimTimerHandle, this, &ABlasterCharacter::ElimAnimFinished, ElimAnimTime); /* change this to respawn convention */
}
void ABlasterCharacter::MulticastEliminated_Implementation() {
	bEliminated = true; /* for animation flow blocking */

	/* disabling all collisions 
	 * must be done on all proxies */
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	GetMesh()->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	/* disabling this character's controller-privileges 
	 * only works on locally owning clients
	 * calling this on server only and expecting on client to work, is doesn't work 
	 * since this is multicast, we don't care */
	DisableInput(GetController<APlayerController>());

	/* stoping any movement 
	 * only works on server
	 * since this is multicast, we don't care */
	GetCharacterMovement()->DisableMovement();
	GetCharacterMovement()->StopMovementImmediately(); /* for any slide, deceleration or free movements happening */

	/* hiding overheadwidget */
	OverheadWidget->SetVisibility(false);

	/* elimination bot 
	 * must do this after removing all movement and input privilages */
	FVector SpawnPoint = GetActorLocation();
	SpawnPoint.Z += 200.f; /* offsetting 200 units up */
	if (ElimParticle) {
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ElimParticle, SpawnPoint, GetActorRotation());
		//ElimParticleComp = UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ElimParticle, SpawnPoint, GetActorRotation());
		//ElimParticleComp->ActivateSystem();
	}
	if (ElimSound) UGameplayStatics::PlaySoundAtLocation(this, ElimSound, SpawnPoint);

	/* playing eliminated animation */
	PlayMontage(ElimMontage, FName("Ascend"));

	/* dissolving our character's mesh */
	DissolveMaterial();
}

void ABlasterCharacter::ElimAnimFinished() {
	/* Happens on Authority */
	if (ABlasterGameMode* BlasterGM = GetWorld()->GetAuthGameMode<ABlasterGameMode>()) {
		/* asking game mode for respawn after elimination animation timer is finished */
		BlasterGM->RequestRespawn(this, Controller);
	}
}

//
//============================================ Dissolve Material ============================================
//
void ABlasterCharacter::DissolveMaterial() {
	/* dynamic material instance
	 * creating dynamic material instance and setting it on character's mesh */
	DissolveMaterialInstanceDynamic = GetMesh()->CreateAndSetMaterialInstanceDynamicFromMaterial(0, DissolveMaterialInstance);
	//DissolveMaterialInstanceDynamic = UMaterialInstanceDynamic::Create(DissolveMaterialInstance, this);
	//GetMesh()->SetMaterial(0, DissolveMaterialInstanceDynamic);

	/* timeline component
	 * using timeline component to get values over time
	 * to update our dissolve dynamic material */
	FOnTimelineFloat DissolveDelegate;
	DissolveDelegate.BindDynamic(this, &ABlasterCharacter::UpdateDissolveMaterial);
	Transition->AddInterpFloat(DissolveCurve, DissolveDelegate);
	Transition->Play();
}
void ABlasterCharacter::UpdateDissolveMaterial(float Value) {
	if (DissolveMaterialInstanceDynamic) {
		DissolveMaterialInstanceDynamic->SetScalarParameterValue(FName("Dissolve"), Value);
	}
}

//
//============================================ Getters ============================================
//
bool ABlasterCharacter::IsInAir() {
	return GetCharacterMovement()->IsFalling();
}
bool ABlasterCharacter::IsAccelerating() {
	return (bool)GetCharacterMovement()->GetCurrentAcceleration().Size();
}
bool ABlasterCharacter::IsWeaponEquipped() {
	return (Combat && Combat->GetEquippedWeapon());
}
bool ABlasterCharacter::IsAiming() {
	return (Combat && Combat->IsAiming());
}

