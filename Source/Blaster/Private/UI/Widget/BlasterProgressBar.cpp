// Copyright Na9i Studio.


#include "UI/Widget/BlasterProgressBar.h"
#include "Components/ProgressBar.h"

void UBlasterProgressBar::NativeTick(const FGeometry& MyGeometry, float InDeltaSeconds) {
	float MainPercent = MainBar->GetPercent(), GhostPercent = GhostBar->GetPercent();
	if (GhostPercent != MainPercent) GhostBar->SetPercent(FMath::FInterpTo(GhostPercent, MainPercent, InDeltaSeconds, 5.f));
}

void UBlasterProgressBar::SetPercent(float CurrentValue, float MaxValue) {
	MainBar->SetPercent(MaxValue != 0.f ? CurrentValue / MaxValue : 0.f);
}