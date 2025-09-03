// Copyright Na9i Studio.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
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
	TObjectPtr<UTextBlock> HealthText;

public:
	/* Health Bar */
	void SetHealth(float CurrentHealth, float MaxHealth);
};
