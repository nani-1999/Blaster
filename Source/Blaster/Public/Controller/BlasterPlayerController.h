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

	virtual void Tick(float DeltaTime) override;
	virtual void LocalTick(float DeltaTime);

	virtual void InitPlayerState() override;

	virtual void OnRep_PlayerState() override;

	/* HUD */
	void UpdateHUDCrosshair(FCrosshairTextures& NewCrosshair, float NewRate, FLinearColor TintColor);
	/* HUD's Overlay */
	void SetHUDOverlayVisibility(bool bVisible);
	void SetHUDOverlayText(EOverlayText TextWidget, float Value);

	ABlasterHUD* GetBlasterHUD() const { return BlasterHUD; }

	/* Match State */
	UFUNCTION(Client, Reliable)
	void ClientOnMatchStateSet(FName State, float StateStartTime, float StateTimeSeconds);

protected:
	virtual void BeginPlay() override;

	//virtual void OnPossess(APawn* aPawn) override;

	/* References */
	ABlasterHUD* BlasterHUD = nullptr;

	/* Server-Client RoundTrip Time */
	float TimeCounter;
	float ServerClientDeltaSecondsUpdateFrequency;
	UFUNCTION(Server, Reliable)
	void ServerRequestServerTime(float ClientTime);
	UFUNCTION(Client, Reliable)
	void ClientReceiveServerTime(float ClientTime, float ServerTime);

	float ServerClientDeltaSeconds = 0;
	float GetServerTime();

	/* Match State */
	FName GM_MatchState;
	float GM_MatchStateStartTime;
	float GM_MatchStateTimeSeconds;

	UFUNCTION(Server, Reliable)
	void ServerRequestMatchState();
};
