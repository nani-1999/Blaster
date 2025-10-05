// Copyright Na9i Studio.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "UI/BlasterUITypes.h"
#include "BlasterPlayerController.generated.h"

class ABlasterHUD;
struct FCrosshairTextures;

UCLASS()
class BLASTER_API ABlasterPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	ABlasterPlayerController();

protected:
	virtual void OnPossess(APawn* aPawn) override;

	/* References */
	ABlasterHUD* BlasterHUD = nullptr;

	/* Server-Client RoundTrip Time */
	float GetServerTime();

	float ServerClientDeltaTime = 0;
	UFUNCTION(Server, Reliable)
	void ServerRequestServerTime(float ClientTime);
	UFUNCTION(Client, Reliable)
	void ClientReceiveServerTime(float ClientTime, float ServerTime);

	float TimeCounter;
	float ServerClientDeltaTimeUpdateFrequency;

public:
	virtual void Tick(float DeltaTime) override;

	//virtual void OnRep_Pawn() override;
	virtual void OnRep_PlayerState() override;

	/* HUD */
	void UpdateHUDCrosshair(FCrosshairTextures& NewCrosshair, float NewRate, FLinearColor TintColor);
	/* HUD's Overlay */
	void SetHUDOverlayText(EOverlayText TextWidget, float Value);

	/* Match State */
	UFUNCTION(Client, Reliable)
	void ClientOnMatchStateSet(FName State);
};
