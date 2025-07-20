// Copyright Na9i Studio.


#include "UI/Widget/TextWidget.h"
#include "Components/TextBlock.h"

void UTextWidget::SetText(FText TextToSet, int32 FontSize) {
	if (DisplayText) {
		FSlateFontInfo FontInfo = DisplayText->GetFont(); /* getting already existing font information, since FSLateFontInfo doesn't have anything by default */
		FontInfo.Size = FontSize;
		DisplayText->SetFont(FontInfo);

		DisplayText->SetText(TextToSet);
	}
}
