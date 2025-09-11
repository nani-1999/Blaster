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

	float CurrentValue;
	float MaxValue;

public:
	FORCEINLINE void SetCurrentValue(float NewCurrentValue) { SetPercent(NewCurrentValue, MaxValue); }
	FORCEINLINE void SetMaxValue(float NewMaxValue) { SetPercent(CurrentValue, NewMaxValue); }
	void SetPercent(float NewCurrentValue, float NewMaxValue);
};
