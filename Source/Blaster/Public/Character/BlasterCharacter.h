// Copyright Na9i Studio.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "BlasterCharacter.generated.h"

class USpringArmComponent;
class UCameraComponent;

class UWidgetComponent;

UCLASS()
class BLASTER_API ABlasterCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	ABlasterCharacter();

protected:
	virtual void BeginPlay() override;

	/* Input Callbacks */
	UFUNCTION()
	void MoveForward(const float Value);
	UFUNCTION()
	void MoveRight(const float Value);
	UFUNCTION()
	void LookUp(const float Value);
	UFUNCTION()
	void Turn(const float Value);

public:
	virtual void Tick(float DeltaTime) override;

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	virtual void PossessedBy(AController* NewController) override;
	virtual void OnRep_PlayerState() override;

	/* Components */
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<USpringArmComponent> CameraBoom;
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UCameraComponent> FollowCamera;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UWidgetComponent> OverheadWidget;

	/* Getters */
	bool IsInAir();
	bool IsAccelerating();
};
