// Copyright Na9i Studio.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "TextWidget.generated.h"

class UTextBlock;

UCLASS()
class BLASTER_API UTextWidget : public UUserWidget
{
	GENERATED_BODY()
	
protected:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> DisplayText;

public:
	void SetText(FText TextToSet);
};
