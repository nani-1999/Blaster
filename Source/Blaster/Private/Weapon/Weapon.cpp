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
#include "Controller/BlasterPlayerController.h"
#include "Kismet/GameplayStatics.h"
#include "Sound/SoundCue.h"
#include "UI/BlasterUITypes.h"

#include "Blaster/Nani/NaniUtility.h"

AWeapon::AWeapon() :
	bCanAim{ false },
	AimedFOV{ 30.f },
	FOVInterpSpeed{ 20.f },
	FireRate{ 0.5f },
	bIsAutomatic{ false },
	AmmoCapacity{ 30 },
	Ammo{ 1 },
	WeaponType{ EWeaponType::EWT_AssaultRifle },
	ReloadTime{ 2.f }
{
	PrimaryActorTick.bCanEverTick = false;
	bReplicates = true;

	/* Weapon Mesh */
	WeaponMesh = CreateDefaultSubobject<USkeletalMeshComponent>("WeaponMesh");
	SetRootComponent(WeaponMesh);

	WeaponMesh->SetCollisionObjectType(ECollisionChannel::ECC_WorldDynamic);
	WeaponMesh->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	WeaponMesh->SetCollisionResponseToChannel(ECollisionChannel::ECC_WorldStatic, ECollisionResponse::ECR_Block);
	WeaponMesh->SetCollisionResponseToChannel(ECollisionChannel::ECC_WorldDynamic, ECollisionResponse::ECR_Block);
	WeaponMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

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
	PickupWidget->SetVisibility(false);
	PickupWidget->SetDrawAtDesiredSize(true);
}

void AWeapon::BeginPlay()
{
	Super::BeginPlay();

	//ServerTest();
	//ClientTest();
	//MulticastTest();

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

void AWeapon::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const {
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AWeapon, WeaponState);
	DOREPLIFETIME(AWeapon, Ammo); 
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
	PickupWidget->SetVisibility(bShow);
}

//
//============================================ Weapon State ============================================
//
void AWeapon::UpdateWeaponState() {
	/* Updates Weapon based on WeaponState */
	switch (WeaponState) {
		case EWeaponState::EWS_Equipped:
			AreaBox->SetCollisionEnabled(ECollisionEnabled::NoCollision); /* this will also triggers end overlap, which sets overlapping weapon to nullptr */
			WeaponMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
			PickupWidget->SetVisibility(false);
			break;
		case EWeaponState::EWS_Dropped:
			AreaBox->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
			//PickupWidget->SetVisibility(true);
			break;
		case EWeaponState::EWS_Initial:
			break;
	}
}

//
//============================================ Socket ============================================
//
FTransform AWeapon::GetForegripTransform() const {
	/* One is Grip and Another One is Foregrip */
	return WeaponMesh->GetSocketTransform(FName("Foregrip")); /* gives world space by default */
}

//
//============================================ Animation ============================================
//
void AWeapon::PlayFireAnimation() {
	/* Playing Weapon Fire Animation */
	if (FireAnimation) WeaponMesh->PlayAnimation(FireAnimation, false);

	/* also Spawning Weapon Shell 
	 * Spawning Casing from WeaponMesh Ammo(Shell) Ejection Socket */
	if (CasingClass) {
		const USkeletalMeshSocket* AmmoEjectSocket = WeaponMesh->GetSocketByName(FName("AmmoEject"));
		if (AmmoEjectSocket) {
			FTransform AmmoEjectSocketTransform = AmmoEjectSocket->GetSocketTransform(WeaponMesh);
			GetWorld()->SpawnActor<ACasing>(CasingClass, AmmoEjectSocketTransform);
		}
	}
}
void AWeapon::PlayFireEmpty() {
	/* Playing Empty Weapon Sound */
	if (FireEmptySound) UGameplayStatics::PlaySoundAtLocation(this, FireEmptySound, GetActorLocation());
}

//
//============================================ Fire Bullet ============================================
//
void AWeapon::FireBullet(const FVector& HitTarget) {
	/* Happens on Authority */

	/* decrementing ammo */
	AddAmmo(-1);
}

//
//============================================ Ammo ============================================
//
void AWeapon::AddAmmo(int32 AddAmount) {
	/* Happens on Authority */

	/* adding ammo */
	Ammo = FMath::Clamp(Ammo + AddAmount, 0, AmmoCapacity);

	/* HUD's Overlay */
	if (BlasterPC) BlasterPC->SetHUDOverlayText(EOverlayText::EOT_Ammo, Ammo);
}
void AWeapon::OnRep_Ammo() {
	NANI_LOG(Warning, "OnRep_Ammo");
	/* HUD */
	if (BlasterPC) BlasterPC->SetHUDOverlayText(EOverlayText::EOT_Ammo, Ammo);
}

//
//============================================ References ============================================
//
void AWeapon::SetOwner(AActor* NewOwner) {
	/* Happens on Authority */
	Super::SetOwner(NewOwner);

	/* This is mainly for reducting computation of casting everytime for setting data on HUD */
	DetermineOwnerLocal(NewOwner);
}
void AWeapon::OnRep_Owner() {
	Super::OnRep_Owner();

	DetermineOwnerLocal(Owner);
}
void AWeapon::DetermineOwnerLocal(AActor* NetLocal) {
	/* classes can only have DetermineOwnerLocal() function if they have anything to do with HUD
	 * for Weapon, Ammo is the case 
	 * function code will be difference based on owner type, on weapon's case owner is always a character */
	if (ACharacter* OwnerChar = Cast<ACharacter>(NetLocal)) {
		if (ABlasterPlayerController* OwnerCharPC = OwnerChar->GetController<ABlasterPlayerController>()) {
			if (OwnerCharPC->IsLocalController()) {
				BlasterPC = OwnerCharPC;
				return;
			}
		}
	}
	BlasterPC = nullptr;
}

void AWeapon::ServerTest_Implementation() {
	NANI_LOG(Error, "%s | ServerTest", *GetName());
}