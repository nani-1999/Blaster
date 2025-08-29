// Copyright Na9i Studio.


#include "Weapon/Weapon.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/BoxComponent.h"
#include "Components/WidgetComponent.h"
#include "UI/Widget/TextWidget.h"
#include "Character/BlasterCharacter.h"
#include "Net/UnrealNetwork.h"
#include "Animation/AnimationAsset.h"
#include "Weapon/Casing.h"
#include "Engine/SkeletalMeshSocket.h"

#include "Blaster/Nani/NaniUtility.h"

AWeapon::AWeapon() :
	AimedFOV{ 30.f },
	FOVInterpSpeed{ 20.f }
{
	PrimaryActorTick.bCanEverTick = false;
	bReplicates = true;

	/* Weapon Mesh */
	WeaponMesh = CreateDefaultSubobject<USkeletalMeshComponent>("WeaponMesh");
	SetRootComponent(WeaponMesh);

	/*  */
	WeaponMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	WeaponMesh->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	WeaponMesh->SetCollisionResponseToChannel(ECollisionChannel::ECC_WorldStatic, ECollisionResponse::ECR_Block);
	WeaponMesh->SetCollisionResponseToChannel(ECollisionChannel::ECC_WorldDynamic, ECollisionResponse::ECR_Block);

	/* Area Box */
	AreaBox = CreateDefaultSubobject<UBoxComponent>("AreaBox");
	AreaBox->SetupAttachment(GetRootComponent());

	AreaBox->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	AreaBox->SetCollisionObjectType(ECollisionChannel::ECC_WorldStatic);
	AreaBox->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	AreaBox->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Overlap);

	/* Pickup Widget */
	PickupWidget = CreateDefaultSubobject<UWidgetComponent>("PickupWidget");
	PickupWidget->SetupAttachment(GetRootComponent());
	PickupWidget->SetWidgetSpace(EWidgetSpace::Screen);
	PickupWidget->SetVisibility(false, true);
	PickupWidget->SetDrawAtDesiredSize(true);
}

void AWeapon::BeginPlay()
{
	Super::BeginPlay();

	//WeaponMesh->SetCollisionEnabled(ECollisionEnabled::PhysicsOnly);
	//WeaponMesh->SetSimulatePhysics(true);
	
	/* Authority */
	if (HasAuthority()) {
		AreaBox->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
		AreaBox->OnComponentBeginOverlap.AddDynamic(this, &AWeapon::AreaBoxBeginOverlap);
		AreaBox->OnComponentEndOverlap.AddDynamic(this, &AWeapon::AreaBoxEndOverlap);
	}

	/* Pickup Widget */
	UTextWidget* TextWidget = Cast<UTextWidget>(PickupWidget->GetUserWidgetObject());
	if (TextWidget) {
		TextWidget->SetText(FText::FromString(FString("Pickup")));
		TextWidget->SetVisibility(ESlateVisibility::HitTestInvisible);
	}
}

void AWeapon::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AWeapon::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const {
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AWeapon, WeaponState);
}

//
//============================================ Overlap ============================================
//
void AWeapon::AreaBoxBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) {
	if (ABlasterCharacter* BlasterCharacter = Cast<ABlasterCharacter>(OtherActor)) {
		BlasterCharacter->SetOverlappingWeapon(this);
	}
}
void AWeapon::AreaBoxEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex) {
	if (ABlasterCharacter* BlasterCharacter = Cast<ABlasterCharacter>(OtherActor)) {
		if (BlasterCharacter->GetOverlappingWeapon() == this) {
			BlasterCharacter->SetOverlappingWeapon(nullptr);
		}
	}
}

//
//============================================ Pickup Widget ============================================
//
void AWeapon::ShowPickupWidget(bool bShow) {
	PickupWidget->SetVisibility(bShow, true);
}

//
//============================================ Weapon State ============================================
//
void AWeapon::UpdateWeaponState() {
	switch (WeaponState) {
		case EWeaponState::EWS_Equipped:
			AreaBox->SetCollisionEnabled(ECollisionEnabled::NoCollision); /* this will also triggers end overlap, which sets overlapping weapon to nullptr */
			WeaponMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
			PickupWidget->SetVisibility(false, true);
			break;
		case EWeaponState::EWS_Dropped:
			AreaBox->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
			break;
		case EWeaponState::EWS_Initial:
			break;
	}
}

//
//============================================ Socket ============================================
//
FTransform AWeapon::GetLeftHandSocketTransform() const {
	return WeaponMesh->GetSocketTransform(FName("LeftHandSocket")); /* getting world space by default */
}

//
//============================================ Animation ============================================
//
void AWeapon::PlayFireAnimation() {
	/* Playing Weapon Fire Animation */
	if (FireAnimation) WeaponMesh->PlayAnimation(FireAnimation, false);

	/* Spawning Weapon Shell */
	if (CasingClass) {
		const USkeletalMeshSocket* AmmoEjectSocket = WeaponMesh->GetSocketByName(FName("AmmoEject"));
		if (AmmoEjectSocket) {
			FTransform AmmoEjectSocketTransform = AmmoEjectSocket->GetSocketTransform(WeaponMesh);
			GetWorld()->SpawnActor<ACasing>(CasingClass, AmmoEjectSocketTransform);
		}
	}
}