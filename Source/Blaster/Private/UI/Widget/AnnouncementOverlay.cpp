// Copyright Na9i Studio.


#include "UI/Widget/AnnouncementOverlay.h"
#include "Components/TextBlock.h"

//void UAnnouncementOverlay::NativeTick(const FGeometry& MyGeometry, float InDeltaTime) {
//	Super::NativeTick(MyGeometry, InDeltaTime);
//
//	CountdownStartSeconds -= InDeltaTime;
//	if (Countdown) Countdown->SetText(FText::AsNumber(CountdownStartSeconds));
//}

void UAnnouncementOverlay::SetCountdownText(FText SecondsText) {
	if (Countdown) Countdown->SetText(SecondsText);
}

void UAnnouncementOverlay::SetMatchState(EAnnouncementMatchState State) {
	if (Title == nullptr || Description == nullptr) return;

	if (State == EAnnouncementMatchState::EAMS_WaitingToStart) {
		Title->SetText(FText::FromString(FString("Match Starts in:")));
		Title->SetVisibility(ESlateVisibility::HitTestInvisible);
		Description->SetText(FText::FromString(FString("Fly Around: W A S D")));
		Description->SetVisibility(ESlateVisibility::HitTestInvisible);
	}
	else if (State == EAnnouncementMatchState::EAMS_InProgress) {
		Title->SetVisibility(ESlateVisibility::Collapsed);
		Description->SetVisibility(ESlateVisibility::Collapsed);
	}
	else if (State == EAnnouncementMatchState::EAMS_WaitingPostMatch) {
		Title->SetText(FText::FromString(FString("New Match Starts in:")));
		Title->SetVisibility(ESlateVisibility::HitTestInvisible);
	}
}