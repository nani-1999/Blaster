// Copyright Na9i Studio.


#include "UI/Widget/BlasterOverlay.h"
#include "Components/TextBlock.h"
#include "UI/Widget/BlasterProgressBar.h"

#include "Blaster/Nani/NaniUtility.h"

//void UBlasterOverlay::SetHealth(float CurrentHealth, float MaxHealth) {
//	/* Health Bar */
//	if (HealthBar) HealthBar->SetPercent(CurrentHealth, MaxHealth);
//
//	/* Health Text Block */
//	if (HealthText) {
//		/* This joins multiple texts with a separator. In this example, a space is inserted between Text1 and Text2
//		 * example: FText Result = FText::Join(FText::FromString(" "), Text1, Text2); */
//		FNumberFormattingOptions FormattingOptions;
//		FormattingOptions.RoundingMode = ERoundingMode::FromZero; /* ceil */
//		FText HealthTxt = FText::Join(FText::FromString("/"), FText::AsNumber(CurrentHealth, &FormattingOptions), FText::AsNumber(MaxHealth, &FormattingOptions));
//		HealthText->SetText(HealthTxt);
//	}
//}
//
//void UBlasterOverlay::SetScore(int32 NewScore) {
//	if (ScoreText) ScoreText->SetText(FText::AsNumber(NewScore));
//}
//
//void UBlasterOverlay::SetDefeats(int32 NewDefeats) {
//	if (DefeatsText) DefeatsText->SetText(FText::AsNumber(NewDefeats));
//}

void UBlasterOverlay::SetText(EOverlayText TextWidget, float Value) {
	/* this is for numberical setter */
	
	/* if this getters bigger, functionalize each case */

	FNumberFormattingOptions* NumberFormat = nullptr;
	UTextBlock* TextBlock = nullptr;

	switch (TextWidget) {
		case EOverlayText::EOT_Ammo : /* since very often and we can fast break */
			TextBlock = AmmoText;
			break;
		case EOverlayText::EOT_Health :
			NumberFormat = new FNumberFormattingOptions();
			NumberFormat->RoundingMode = ERoundingMode::FromZero; /* ceil */
			TextBlock = HealthText;
			if (HealthBar) HealthBar->SetCurrentValue(Value);
			break;
		case EOverlayText::EOT_MaxHealth :
			NumberFormat = new FNumberFormattingOptions();
			NumberFormat->RoundingMode = ERoundingMode::FromZero; /* ceil */
			TextBlock = MaxHealthText;
			if (HealthBar) HealthBar->SetMaxValue(Value);
			break;
		case EOverlayText::EOT_CarriedAmmo :
			TextBlock = CarriedAmmoText;
			break;
		case EOverlayText::EOT_Score :
			TextBlock = ScoreText;
			break;
		case EOverlayText::EOT_Elims :
			TextBlock = ElimsText;
			break;
		case EOverlayText::EOT_Defeats :
			TextBlock = DefeatsText;
			break;
	}

	if (TextBlock) TextBlock->SetText(FText::AsNumber(Value, NumberFormat));
}