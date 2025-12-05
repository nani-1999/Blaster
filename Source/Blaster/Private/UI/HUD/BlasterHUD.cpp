// Copyright Na9i Studio.


#include "UI/HUD/BlasterHUD.h"
#include "UI/Widget/BlasterOverlay.h"
#include "UI/Widget/TextWidget.h"
#include "UI/Widget/AnnouncementOverlay.h"

#include "Blaster/Nani/NaniUtility.h"

void ABlasterHUD::SetOwner(AActor* NewOwner) {
	Super::SetOwner(NewOwner);

	/* after lots of trial and errors from different classes, there is where its safe to built overlay */
	APlayerController* PC = GetOwner<APlayerController>();
	CreateOverlay(PC); /* a hud's owner is always a controller */
	CreateCountdownText(PC);
	CreateAnnouncementOverlay(PC);
}

void ABlasterHUD::DrawHUD() {
	Super::DrawHUD();

	/* size of viewport */
	FVector2D ViewportSize;
	if (GEngine == nullptr || GEngine->GameViewport == nullptr) return;
	GEngine->GameViewport->GetViewportSize(ViewportSize);
	/* center of viewport */
	FVector2D ViewportCenter = ViewportSize / 2.f;

	/* offseting for crosshair drawing */
	FVector2D CrosshairDrawLoc = ViewportCenter - (CrosshairSize / 2.f);
	/* drawing crosshair */
	DrawCrosshair(CrosshairDrawLoc);
}

void ABlasterHUD::DrawCrosshair(FVector2D DrawLoc) {
	/* Center */
	if (Crosshair.Center) DrawTexture(Crosshair.Center, DrawLoc.X, DrawLoc.Y, CrosshairSize, CrosshairSize, 0.f, 0.f, 1.f, 1.f, CrosshairTintColor);
	/* Top */
	float CrosshairTextureTopLocationY = DrawLoc.Y - (CrosshairSize * CrosshairSpreadRate);
	if (Crosshair.Top) DrawTexture(Crosshair.Top, DrawLoc.X, CrosshairTextureTopLocationY, CrosshairSize, CrosshairSize, 0.f, 0.f, 1.f, 1.f, CrosshairTintColor);
	/* Right */
	float CrosshairTextureRightLocationX = DrawLoc.X + (CrosshairSize * CrosshairSpreadRate);
	if (Crosshair.Right) DrawTexture(Crosshair.Right, CrosshairTextureRightLocationX, DrawLoc.Y, CrosshairSize, CrosshairSize, 0.f, 0.f, 1.f, 1.f, CrosshairTintColor);
	/* Bottom */
	float CrosshairTextureBottomLocationY = DrawLoc.Y + (CrosshairSize * CrosshairSpreadRate);
	if (Crosshair.Bottom) DrawTexture(Crosshair.Bottom, DrawLoc.X, CrosshairTextureBottomLocationY, CrosshairSize, CrosshairSize, 0.f, 0.f, 1.f, 1.f, CrosshairTintColor);
	/* Left */
	float CrosshairTextureLeftLocationX = DrawLoc.X - (CrosshairSize * CrosshairSpreadRate);
	if (Crosshair.Left) DrawTexture(Crosshair.Left, CrosshairTextureLeftLocationX, DrawLoc.Y, CrosshairSize, CrosshairSize, 0.f, 0.f, 1.f, 1.f, CrosshairTintColor);
}

//
//============================================ Countdown ============================================
//
void ABlasterHUD::CreateCountdownText(APlayerController* Controller) {
	/* use enum type if there are lot of widgets to create on hud */

	if (CountdownTextClass && Controller) {
		CountdownText = CreateWidget<UTextWidget>(Controller, CountdownTextClass, FName("CountdownText"));
		if (CountdownText) {
			CountdownText->AddToViewport();
			CountdownText->SetVisibility(ESlateVisibility::HitTestInvisible);
		}
	}
}
void ABlasterHUD::SetCountdownText(float Seconds) {
	if (CountdownText) {
		int MinVal = Seconds / 60.f;
		int SecVal = (int)Seconds % 60;
		FText CountdownTxt = FText::FromString(FString::Printf(TEXT("%2d:%2d"), MinVal, SecVal));
		CountdownText->SetText(CountdownTxt);
	}
}

//
//============================================ Overlay ============================================
//
void ABlasterHUD::CreateOverlay(APlayerController* Controller) {
	if (OverlayClass && Controller) {
		Overlay = CreateWidget<UBlasterOverlay>(Controller, OverlayClass, FName("BlasterOverlay"));
		if (Overlay) {
			Overlay->AddToViewport();
			Overlay->SetVisibility(ESlateVisibility::Hidden);
		}
	}
}
void ABlasterHUD::SetOverlayVisibility(bool bVisible) {
	if (Overlay) Overlay->SetVisibility(bVisible ? ESlateVisibility::HitTestInvisible : ESlateVisibility::Hidden);
}
void ABlasterHUD::SetOverlayText(EOverlayText TextWidget, float Value) {
	if (Overlay) Overlay->SetText(TextWidget, Value);
}

//
//============================================ Announcement Overlay ============================================
//
void ABlasterHUD::CreateAnnouncementOverlay(APlayerController* Controller) {
	if (AnnouncementOverlayClass && Controller) {
		AnnouncementOverlay = CreateWidget<UAnnouncementOverlay>(Controller, AnnouncementOverlayClass, FName("AnnouncementOverlay"));
		if (AnnouncementOverlay) {
			AnnouncementOverlay->AddToViewport();
			AnnouncementOverlay->SetVisibility(ESlateVisibility::HitTestInvisible);
		}
	}
}
void ABlasterHUD::SetAnnouncementOverlayVisibility(bool bVisibility) {
	if (AnnouncementOverlay) AnnouncementOverlay->SetVisibility(bVisibility ? ESlateVisibility::HitTestInvisible : ESlateVisibility::Hidden);
}
void ABlasterHUD::SetAnnouncementMatchState(EAnnouncementMatchState State) {
	if (AnnouncementOverlay) AnnouncementOverlay->SetMatchState(State);
}
void ABlasterHUD::SetAnnouncementCountdown(float Seconds) {
	if (AnnouncementOverlay) {
		int MinVal = Seconds / 60.f;
		int SecVal = (int)Seconds % 60;
		FText CountdownTxt = FText::FromString(FString::Printf(TEXT("%2d:%2d"), MinVal, SecVal));
		AnnouncementOverlay->SetCountdownText(CountdownTxt);
	} 
}