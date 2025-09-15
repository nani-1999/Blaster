// Copyright Na9i Studio.


#include "UI/HUD/BlasterHUD.h"
#include "UI/Widget/BlasterOverlay.h"

#include "Blaster/Nani/NaniUtility.h"

void ABlasterHUD::SetOwner(AActor* NewOwner) {
	Super::SetOwner(NewOwner);

	/* after lots of trial and errors from different classes, there is where its safe to built overlay */
	InitOverlay(GetOwner<APlayerController>()); /* a hud's owner is always a controller */
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
//============================================ Overlay ============================================
//
void ABlasterHUD::InitOverlay(APlayerController* Controller) {
	if (OverlayClass && Controller) {
		Overlay = CreateWidget<UBlasterOverlay>(Controller, OverlayClass, FName("BlasterOverlay"));
		if (Overlay) {
			Overlay->AddToViewport();
			Overlay->SetVisibility(ESlateVisibility::HitTestInvisible);
		}
	}
}
void ABlasterHUD::SetOverlayText(EOverlayText TextWidget, float Value) {
	if (Overlay) Overlay->SetText(TextWidget, Value);
}