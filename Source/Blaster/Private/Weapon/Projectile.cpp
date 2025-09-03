// Copyright Na9i Studio.


#include "Weapon/Projectile.h"
#include "Components/BoxComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Particles/ParticleSystemComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Particles/ParticleSystem.h"
#include "Sound/SoundCue.h"

#include "Blaster/Nani/NaniUtility.h"

AProjectile::AProjectile() {
	PrimaryActorTick.bCanEverTick = false;
	bReplicates = true;

	/* maybe we can not replicate, and do multicast hittarget in projectileweapon
	 * this will make the bullet travel(projectiles) to the point in client and server without actually replicating the bullet */

	BoxCollision = CreateDefaultSubobject<UBoxComponent>("BoxCollision");
	SetRootComponent(BoxCollision);

	BoxCollision->SetCollisionObjectType(ECollisionChannel::ECC_WorldDynamic);
	BoxCollision->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	BoxCollision->SetCollisionResponseToChannel(ECollisionChannel::ECC_WorldDynamic, ECollisionResponse::ECR_Block);
	BoxCollision->SetCollisionResponseToChannel(ECollisionChannel::ECC_WorldStatic, ECollisionResponse::ECR_Block);
	BoxCollision->SetCollisionResponseToChannel(ECollisionChannel::ECC_PhysicsBody, ECollisionResponse::ECR_Block); /* ECR_Block for OnHit() to work */
	//BoxCollision->SetCollisionResponseToChannel(ECollisionChannel::ECC_Visibility, ECollisionResponse::ECR_Block);
	BoxCollision->SetCollisionEnabled(ECollisionEnabled::QueryOnly);

	ProjectileComp = CreateDefaultSubobject<UProjectileMovementComponent>("ProjectileComp");
	ProjectileComp->bRotationFollowsVelocity = true; /* rotation follow direction of velocity, instead of fixed actor's rotation */
	ProjectileComp->InitialSpeed = 15000.f;
	ProjectileComp->MaxSpeed = 15000.f;
	//ProjectileComp->ProjectileGravityScale = 0.f;

	TracerParticle = CreateDefaultSubobject<UParticleSystemComponent>("TracerParticle");
	TracerParticle->SetupAttachment(BoxCollision);
}

void AProjectile::BeginPlay()
{
	Super::BeginPlay();
	
	if (HasAuthority()) BoxCollision->OnComponentHit.AddDynamic(this, &AProjectile::OnHit); /* for ECR_Block */
	//BoxCollision->OnComponentBeginOverlap.AddDynamic(this, &AProjectile::OnBeginOverlap); /* for ECR_Overlap */
}

void AProjectile::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AProjectile::OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit) {
	NANI_LOG(Warning, "%s Hits %s", *HitComponent->GetName(), *OtherComp->GetName());
	/* if we destroy a replicated actor on client
	 * the actor still exist on server 
	 * means on client the actor channel is still exist on its netdriver
	 * which might trigger respawning of actor, this happens very fast we might not know the difference 
	 * so destroy replicated actor only on server */
	Destroy();
}

void AProjectile::Destroyed() {
	Super::Destroyed();

	/* cannot do this particle and sound in override Destroyed() function, since net relevancy is a factor
	 * but we doing it anyway since it saves multicast rpc or collision hit event on client 
	 * will change/fix this if found any problem in future */
	if (ImpactParticle) UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ImpactParticle, GetActorTransform());
	if (ImpactSound) UGameplayStatics::PlaySoundAtLocation(this, ImpactSound, GetActorLocation());
}