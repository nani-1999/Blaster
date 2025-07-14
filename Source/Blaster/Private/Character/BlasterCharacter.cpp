// Copyright Na9i Studio.


#include "Character/BlasterCharacter.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/CharacterMovementComponent.h"

#include "Components/WidgetComponent.h"
#include "UI/Widget/TextWidget.h"
#include "GameFramework/PlayerState.h"

#include "Blaster/Nani/NaniUtility.h"

ABlasterCharacter::ABlasterCharacter()
{
	PrimaryActorTick.bCanEverTick = false;

	CameraBoom = CreateDefaultSubobject<USpringArmComponent>("CameraBoom");
	CameraBoom->SetupAttachment(GetMesh());
	CameraBoom->SetUsingAbsoluteRotation(true);
	CameraBoom->TargetArmLength = 600.f;
	CameraBoom->bUsePawnControlRotation = true;

	FollowCamera = CreateDefaultSubobject<UCameraComponent>("FollowCamera");
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	//FollowCamera->bUsePawnControlRotation = false;
	
	bUseControllerRotationYaw = false; /* since other two axis are false by default */
	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate = FRotator(0.f, 420.f, 0.f);

	/* Overhead Widget */
	OverheadWidget = CreateDefaultSubobject<UWidgetComponent>("OverheadWidget");
	OverheadWidget->SetupAttachment(GetRootComponent());
	OverheadWidget->SetWidgetSpace(EWidgetSpace::Screen);
	OverheadWidget->SetVisibility(false, true);
	OverheadWidget->SetDrawAtDesiredSize(true);
}

void ABlasterCharacter::BeginPlay()
{
	Super::BeginPlay();

}

void ABlasterCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

//
//============================================ Player State ============================================
//
void ABlasterCharacter::PossessedBy(AController* NewController) {
	Super::PossessedBy(NewController);

	/* Overhead Widget */
	UTextWidget* TextWidget = Cast<UTextWidget>(OverheadWidget->GetUserWidgetObject());
	if (TextWidget) {
		FString NetRoleStr = GetNetRoleStr<FString>(GetLocalRole());
		FString PlayerName = GetPlayerState<APlayerState>()->GetPlayerName();
		FString TextStr = NetRoleStr + "\n" + PlayerName;

		TextWidget->SetText(FText::FromString(TextStr));
		TextWidget->SetVisibility(ESlateVisibility::HitTestInvisible);
		OverheadWidget->SetVisibility(true, true);

	}
}
void ABlasterCharacter::OnRep_PlayerState() {
	Super::OnRep_PlayerState();

	/* Overhead Widget */
	UTextWidget* TextWidget = Cast<UTextWidget>(OverheadWidget->GetUserWidgetObject());
	if (TextWidget) {
		FString NetRoleStr = GetNetRoleStr<FString>(GetLocalRole());
		FString PlayerName = GetPlayerState<APlayerState>()->GetPlayerName();
		FString TextStr = NetRoleStr + "\n" + PlayerName;

		TextWidget->SetText(FText::FromString(TextStr));
		TextWidget->SetVisibility(ESlateVisibility::HitTestInvisible);
		OverheadWidget->SetVisibility(true, true);
	}
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

	PlayerInputComponent->BindAction("Jump", EInputEvent::IE_Pressed, this, &ACharacter::Jump);
}

void ABlasterCharacter::MoveForward(const float Value) {
	if (Value && Controller) {
		UE_LOG(LogTemp, Warning, TEXT("MoveForward"));
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

//
//============================================ Getters ============================================
//
bool ABlasterCharacter::IsInAir() {
	return GetCharacterMovement()->IsFalling();
}
bool ABlasterCharacter::IsAccelerating() {
	return (bool)GetCharacterMovement()->GetCurrentAcceleration().Size();
}