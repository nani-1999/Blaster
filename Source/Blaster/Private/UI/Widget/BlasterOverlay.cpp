// Copyright Na9i Studio.


#include "UI/Widget/BlasterOverlay.h"
#include "Components/TextBlock.h"
#include "UI/Widget/BlasterProgressBar.h"

#include "Blaster/Nani/NaniUtility.h"

//
//============================================ Health Bar ============================================
//
void UBlasterOverlay::SetHealth(float CurrentHealth, float MaxHealth) {
	/* Health Bar */
	if (HealthBar) HealthBar->SetPercent(CurrentHealth, MaxHealth);

	/* Health Text Block */
	if (HealthText) {
		/* This joins multiple texts with a separator. In this example, a space is inserted between Text1 and Text2 */
		//FText Result = FText::Join(FText::FromString(" "), Text1, Text2);
		FNumberFormattingOptions FormattingOptions;
		FormattingOptions.RoundingMode = ERoundingMode::FromZero;
		FText HealthTxt = FText::Join(FText::FromString("/"), FText::AsNumber(CurrentHealth, &FormattingOptions), FText::AsNumber(MaxHealth, &FormattingOptions));
		HealthText->SetText(HealthTxt);
	}
}