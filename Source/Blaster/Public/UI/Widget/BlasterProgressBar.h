// Copyright Na9i Studio.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "BlasterProgressBar.generated.h"

class UProgressBar;

UCLASS()
class BLASTER_API UBlasterProgressBar : public UUserWidget
{
	GENERATED_BODY()
	
protected:
	/* Tick */
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UProgressBar> MainBar;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UProgressBar> GhostBar;

public:
	void SetPercent(float CurrentValue, float MaxValue);
};
