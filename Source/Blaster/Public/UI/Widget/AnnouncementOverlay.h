// Copyright Na9i Studio.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "UI/BlasterUITypes.h"
#include "AnnouncementOverlay.generated.h"

class UTextBlock;

UCLASS()
class BLASTER_API UAnnouncementOverlay : public UUserWidget
{
	GENERATED_BODY()
	
protected:
	//virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> Title;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> Countdown;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> Description;

	void Collapse(bool bCol);

public:
	void SetMatchState(EAnnouncementMatchState State);
	void SetCountdownText(FText SecondsText);
};
