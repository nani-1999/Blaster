// Copyright Na9i Studio.


#include "Weapon/Casing.h"
#include "Components/StaticMeshComponent.h"
#include "Sound/SoundCue.h"
#include "Kismet/GameplayStatics.h"

#include "Blaster/Nani/NaniUtility.h"

ACasing::ACasing() :
	ShellEjectImpulse{ 10.f }
{
	PrimaryActorTick.bCanEverTick = false;

	ShellMesh = CreateDefaultSubobject<UStaticMeshComponent>("ShellMesh");
	SetRootComponent(ShellMesh);

	//ShellMesh->SetCollisionObjectType(ECollisionChannel::ECC_WorldStatic);
	ShellMesh->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	ShellMesh->SetCollisionResponseToChannel(ECollisionChannel::ECC_WorldDynamic, ECollisionResponse::ECR_Block);
	ShellMesh->SetCollisionResponseToChannel(ECollisionChannel::ECC_WorldStatic, ECollisionResponse::ECR_Block);

	/* since query overlapping event won't occured on channels that are blocked response, 
	 * because collision is not actually overlapping into the other collision, but blocking
	 * so we use rigidbody which triggers hit events
	 * but this can cause multiple hit events to trigger, because of rollovers, because of physics
	 * so we prevent multiple hit events with disabling rigidbody once we get hit event
	 * here we are using rigidbody and physicsonly, events magically triggered without even quary */
	ShellMesh->SetNotifyRigidBodyCollision(true); /* blueprint naming is 'simulation generates hit events' */
	ShellMesh->SetCollisionEnabled(ECollisionEnabled::PhysicsOnly); /* by default value of staticmesh is query and physics */
	ShellMesh->SetSimulatePhysics(true);
	ShellMesh->SetEnableGravity(true);
}

// Called when the game starts or when spawned
void ACasing::BeginPlay()
{
	Super::BeginPlay();
	
	ShellMesh->OnComponentHit.AddDynamic(this, &ACasing::OnHit);

	ShellMesh->AddImpulse(GetActorForwardVector() * ShellEjectImpulse);

	SetLifeSpan(5.f);
}

void ACasing::OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit) {
	if (ShellFallenSound) UGameplayStatics::PlaySoundAtLocation(this, ShellFallenSound, GetActorLocation());

	/* to prevent from getting multiple hit events */
	ShellMesh->SetNotifyRigidBodyCollision(false);
}