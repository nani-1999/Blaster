// Copyright Na9i Studio.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "BlasterHUD.generated.h"

USTRUCT()
struct FCrosshairTextures {

	GENERATED_BODY()

	UPROPERTY()
	TObjectPtr<UTexture2D> Center;
	UPROPERTY()
	TObjectPtr<UTexture2D> Top;
	UPROPERTY()
	TObjectPtr<UTexture2D> Right;
	UPROPERTY()
	TObjectPtr<UTexture2D> Bottom;
	UPROPERTY()
	TObjectPtr<UTexture2D> Left;
};

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

public:
	virtual void DrawHUD() override;

	/* Crosshair */
	FORCEINLINE void UpdateCrosshair(FCrosshairTextures& NewCrosshair, float NewRate, FLinearColor TintColor) { 
		Crosshair = NewCrosshair; 
		CrosshairSpreadRate = NewRate; 
		CrosshairTintColor = TintColor; 
	}
};
