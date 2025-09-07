// Copyright Na9i Studio.


#include "Controller/BlasterPlayerController.h"
#include "UI/HUD/BlasterHUD.h"

#include "Blaster/Nani/NaniUtility.h"

//
//============================================ HUD's Overlay ============================================
//
void ABlasterPlayerController::SetupHUDOverlay() {
	if (ABlasterHUD* BlasterHUD = GetHUD<ABlasterHUD>()) {
		BlasterHUD->SetupOverlay(this);
	}
}
void ABlasterPlayerController::SetHUDOverlayHealth(float CurrentHealth, float MaxHealth) {
	if (ABlasterHUD* BlasterHUD = GetHUD<ABlasterHUD>()) {
		BlasterHUD->SetOverlayHealth(CurrentHealth, MaxHealth);
	}
}
void ABlasterPlayerController::SetHUDOverlayScore(float NewScore) {
	if (ABlasterHUD* BlasterHUD = GetHUD<ABlasterHUD>()) {
		BlasterHUD->SetOverlayScore(NewScore);
	}
}
void ABlasterPlayerController::SetHUDOverlayDefeats(int32 NewDefeats) {
	if (ABlasterHUD* BlasterHUD = GetHUD<ABlasterHUD>()) {
		BlasterHUD->SetOverlayDefeats(NewDefeats);
	}
}