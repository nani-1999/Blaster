// Copyright Na9i Studio.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "UI/BlasterUITypes.h"
#include "BlasterOverlay.generated.h"

class UTextBlock;
class UBlasterProgressBar;

UCLASS()
class BLASTER_API UBlasterOverlay : public UUserWidget
{
	GENERATED_BODY()
	
protected:
	/* Health Bar */
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UBlasterProgressBar> HealthBar;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> MaxHealthText;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> HealthText;

	/* Score */
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> ScoreText;

	/* Elims */
	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UTextBlock> ElimsText;

	/* Defeats */
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> DefeatsText;

	/* Ammo */
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> AmmoText;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> CarriedAmmoText;

public:
	void SetText(EOverlayText TextWidget, float Value);
};
