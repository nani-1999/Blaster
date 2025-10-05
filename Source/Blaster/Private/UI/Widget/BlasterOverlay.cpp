// Copyright Na9i Studio.


#include "UI/Widget/BlasterOverlay.h"
#include "Components/TextBlock.h"
#include "UI/Widget/BlasterProgressBar.h"

#include "Blaster/Nani/NaniUtility.h"

void UBlasterOverlay::SetText(EOverlayText TextWidget, float Value) {
	/* this is for numberical setter */
	
	/* if this getters bigger, functionalize each case */

	FNumberFormattingOptions* NumberFormat = nullptr;
	UTextBlock* TextBlock = nullptr;

	switch (TextWidget) {
		case EOverlayText::EOT_CountDown :
			SetCountDownText(Value);
			return;
			break;
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

void UBlasterOverlay::SetCountDownText(float Seconds) {
	int MinVal = Seconds / 60.f;
	int SecVal = (int)Seconds % 60;

	FText CountDownTxt = FText::FromString(FString::Printf(TEXT("%2d:%2d"), MinVal, SecVal));

	if (CountDownText) CountDownText->SetText(CountDownTxt);
}