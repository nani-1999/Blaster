// Copyright Na9i Studio.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Casing.generated.h"

class UStaticMeshComponent;
class USoundCue;

UCLASS()
class BLASTER_API ACasing : public AActor
{
	GENERATED_BODY()
	
public:	
	ACasing();

protected:
	virtual void BeginPlay() override;

	/* Mesh */
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UStaticMeshComponent> ShellMesh;

	/* Force */
	UPROPERTY(EditDefaultsOnly)
	float ShellEjectImpulse;

	/* Sound */
	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<USoundCue> ShellFallenSound;

	/* Events */
	UFUNCTION()
	void OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);
};
