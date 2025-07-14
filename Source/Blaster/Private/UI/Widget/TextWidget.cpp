// Copyright Na9i Studio.


#include "UI/Widget/TextWidget.h"
#include "Components/TextBlock.h"

void UTextWidget::SetText(FText TextToSet) {
	if (DisplayText) DisplayText->SetText(TextToSet);
}
