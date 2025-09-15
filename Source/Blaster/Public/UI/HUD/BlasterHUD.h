// Copyright Na9i Studio.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "UI/BlasterUITypes.h"
#include "BlasterHUD.generated.h"

class UBlasterOverlay;

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

	/* Overlay */
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UBlasterOverlay> OverlayClass;
	UPROPERTY()
	TObjectPtr<UBlasterOverlay> Overlay;
	void InitOverlay(APlayerController* Controller);

public:
	virtual void SetOwner(AActor* NewOwner) override;

	virtual void DrawHUD() override;

	/* Crosshair */
	FORCEINLINE void UpdateCrosshair(FCrosshairTextures& NewCrosshair, float NewRate, FLinearColor TintColor) { 
		Crosshair = NewCrosshair; 
		CrosshairSpreadRate = NewRate; 
		CrosshairTintColor = TintColor; 
	}

	/* Overlay */
	void SetOverlayText(EOverlayText OverlayText, float Value);
};
