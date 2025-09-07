// Copyright Na9i Studio.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "BlasterOverlay.generated.h"

//UENUM()
//enum class EOverlayText : uint8 {
//	EOT_Health,
//	EOT_Score,
//	EOT_Elims,
//	EOT_Defeats,
//	EOT_MAX
//};

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

public:
	void SetHealth(float CurrentHealth, float MaxHealth);
	void SetScore(float NewScore);
	void SetDefeats(int32 NewDefeats);
};
