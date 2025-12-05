// Copyright Na9i Studio.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "UI/BlasterUITypes.h"
#include "BlasterHUD.generated.h"

class UBlasterOverlay;
class UTextWidget;
class UAnnouncementOverlay;

UCLASS()
class BLASTER_API ABlasterHUD : public AHUD
{
	GENERATED_BODY()
	
protected:
	/* Crosshair */
	FCrosshairTextures Crosshair;

	float CrosshairSpreadRate;
	FLinearColor CrosshairTintColor;

	UPROPERTY(EditDefaultsOnly)
	float CrosshairSize = 32.f;

	void DrawCrosshair(FVector2D DrawLoc);

	/* Countdown */
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UTextWidget> CountdownTextClass;
	UPROPERTY()
	TObjectPtr<UTextWidget> CountdownText;

	/* Announcement Overlay */
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UAnnouncementOverlay> AnnouncementOverlayClass;
	UPROPERTY()
	TObjectPtr<UAnnouncementOverlay> AnnouncementOverlay;

	/* Overlay */
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UBlasterOverlay> OverlayClass;
	UPROPERTY()
	TObjectPtr<UBlasterOverlay> Overlay;

public:
	virtual void SetOwner(AActor* NewOwner) override;

	virtual void DrawHUD() override;

	/* Crosshair */
	FORCEINLINE void UpdateCrosshair(FCrosshairTextures& NewCrosshair, float NewRate, FLinearColor TintColor) { 
		Crosshair = NewCrosshair; 
		CrosshairSpreadRate = NewRate; 
		CrosshairTintColor = TintColor; 
	}

	/* Countdown */
	void CreateCountdownText(APlayerController* Controller);
	void SetCountdownText(float Seconds);

	/* Overlay */
	void CreateOverlay(APlayerController* Controller);
	void SetOverlayVisibility(bool bVisible);
	void SetOverlayText(EOverlayText OverlayText, float Value);

	/* Announcement Overlay */
	void CreateAnnouncementOverlay(APlayerController* Controller);
	void SetAnnouncementOverlayVisibility(bool bVisibility);
	void SetAnnouncementMatchState(EAnnouncementMatchState State);
	void SetAnnouncementCountdown(float Seconds);
};
