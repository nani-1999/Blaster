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

#include "Blaster/Nani/NaniUtility.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SkeletalMeshComponent.h"

ABlasterCharacter::ABlasterCharacter()
{
	PrimaryActorTick.bCanEverTick = false;
	/* Network */
	NetUpdateFrequency = 66.f;
	MinNetUpdateFrequency = 33.f;

	/* Unblocking Camera over Pawn */
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);
	GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);

	/* Crosshair Color */
	GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Visibility, ECollisionResponse::ECR_Block);

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
	OverheadWidget->SetVisibility(false, true);
	OverheadWidget->SetDrawAtDesiredSize(true);

	/* Test */
	TestMesh = CreateDefaultSubobject<UStaticMeshComponent>("TestMesh");
	TestMesh->SetupAttachment(GetRootComponent());
	TestMesh->SetUsingAbsoluteRotation(true);
	TestMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	WeaponMesh = CreateDefaultSubobject<USkeletalMeshComponent>("WeaponMesh");
	WeaponMesh->SetupAttachment(GetMesh(), FName("RightHandSocket"));
	WeaponMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	/* Crouch 
	 * enabling crouch
	 * also crouch is auto replicated since its handled by CharacterMovement 
	 * replication triggered by changing bWantsToCrouch, it also checking if can crouch */
	GetCharacterMovement()->NavAgentProps.bCanCrouch = true;
	GetCharacterMovement()->SetCrouchedHalfHeight(60.f); /* accessing CrouchedHalfHeight directly is depricated, using set instead */
	GetCharacterMovement()->MaxWalkSpeedCrouched = 200.f;
}

void ABlasterCharacter::BeginPlay()
{
	Super::BeginPlay();

	/* must be in beginplay not in object creation, since we need to know network role 
	 * combat component needs camera for fov interping while aiming */
	if (IsLocallyControlled()) Combat->SetCamera(FollowCamera);
}

void ABlasterCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (Controller) {
		TestMesh->SetWorldRotation(Controller->GetControlRotation());
	}
}

//void ABlasterCharacter::PostInitializeComponents() {
//	Super::PostInitializeComponents();
//
//	if (Combat) {
//		Combat->Character = this;
//	}
//}

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
//============================================ Replication ============================================
//
void ABlasterCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const {
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION(ABlasterCharacter, OverlappingWeapon, COND_OwnerOnly); /* replicates to owning client only */
}

//
//============================================ Overhead Widget ============================================
//
void ABlasterCharacter::SetupOverheadWidget() {
	/* Overhead Widget */
	UTextWidget* TextWidget = Cast<UTextWidget>(OverheadWidget->GetUserWidgetObject());
	if (TextWidget) {
		FString NetRoleStr = GetNetRoleStr<FString>(GetLocalRole());
		FString PlayerName = GetPlayerState<APlayerState>()->GetPlayerName();
		FString TextStr = NetRoleStr + "\n" + PlayerName;

		TextWidget->SetText(FText::FromString(TextStr), 20);
		TextWidget->SetVisibility(ESlateVisibility::HitTestInvisible);
		OverheadWidget->SetVisibility(true, true);
	}
}
void ABlasterCharacter::PossessedBy(AController* NewController) {
	Super::PossessedBy(NewController);

	SetupOverheadWidget();
}
void ABlasterCharacter::OnRep_PlayerState() {
	Super::OnRep_PlayerState();

	SetupOverheadWidget();
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
	//NANI_LOG(Warning, "TestPressed");
	ServerTestPressed();
}
void ABlasterCharacter::ServerTestPressed_Implementation() {
	//NANI_LOG(Warning, "ServerTestPressed");
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
	//if (Combat) Combat->SetFiring(false);
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