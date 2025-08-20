// Copyright Na9i Studio.


#include "Weapon/Projectile.h"
#include "Components/BoxComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Particles/ParticleSystemComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Particles/ParticleSystem.h"
#include "Sound/SoundCue.h"

AProjectile::AProjectile() {
	PrimaryActorTick.bCanEverTick = false;
	bReplicates = true;

	/* maybe we can not replicate, and do multicast hittarget in projectileweapon
	 * this will make the bullet travel(projectiles) to the point in client and server without actually replicating the bullet */

	BoxCollision = CreateDefaultSubobject<UBoxComponent>("BoxCollision");
	SetRootComponent(BoxCollision);

	BoxCollision->SetCollisionObjectType(ECollisionChannel::ECC_WorldDynamic);
	BoxCollision->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	BoxCollision->SetCollisionResponseToChannel(ECollisionChannel::ECC_Visibility, ECollisionResponse::ECR_Block);
	BoxCollision->SetCollisionResponseToChannel(ECollisionChannel::ECC_WorldStatic, ECollisionResponse::ECR_Block);
	BoxCollision->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);

	ProjectileComp = CreateDefaultSubobject<UProjectileMovementComponent>("ProjectileComp");
	ProjectileComp->bRotationFollowsVelocity = true; /* rotation follow direction of velocity, instead of fixed actor's rotation */
	ProjectileComp->InitialSpeed = 11000.f;
	ProjectileComp->MaxSpeed = 11000.f;
	//ProjectileComp->ProjectileGravityScale = 0.f;

	TracerParticle = CreateDefaultSubobject<UParticleSystemComponent>("TracerParticle");
	TracerParticle->SetupAttachment(BoxCollision);
}

void AProjectile::BeginPlay()
{
	Super::BeginPlay();
	
	BoxCollision->OnComponentHit.AddDynamic(this, &AProjectile::OnHit);
}

void AProjectile::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AProjectile::OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit) {
	/* cannot do this particle and sound in Destroyed() function, since net relevancy is a factor */
	if (ImpactParticle) UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ImpactParticle, GetActorTransform());
	if (ImpactSound) UGameplayStatics::PlaySoundAtLocation(this, ImpactSound, GetActorLocation());

	if (HasAuthority()) Destroy();
}
