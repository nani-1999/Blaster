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
void ABlasterPlayerController::SetHUDOverlayText(EOverlayText TextWidget, float Value) {
	if (ABlasterHUD* BlasterHUD = GetHUD<ABlasterHUD>()) {
		BlasterHUD->SetOverlayText(TextWidget, Value);
	}
}
//void ABlasterPlayerController::SetHUDOverlayMaxHealth(float NewVal) {
//	/* all these shitty functions because i don't know how to access EOverlayText enum class here */
//	if (ABlasterHUD* BlasterHUD = GetHUD<ABlasterHUD>()) {
//		BlasterHUD->SetOverlayMaxHealth(NewVal);
//	}
//}
//void ABlasterPlayerController::SetHUDOverlayHealth(float NewVal) {
//	if (ABlasterHUD* BlasterHUD = GetHUD<ABlasterHUD>()) {
//		BlasterHUD->SetOverlayHealth(NewVal);
//	}
//}
//void ABlasterPlayerController::SetHUDOverlayScore(float NewVal) {
//	if (ABlasterHUD* BlasterHUD = GetHUD<ABlasterHUD>()) {
//		BlasterHUD->SetOverlayScore(NewVal);
//	}
//}
//void ABlasterPlayerController::SetHUDOverlayDefeats(int32 NewVal) {
//	if (ABlasterHUD* BlasterHUD = GetHUD<ABlasterHUD>()) {
//		BlasterHUD->SetOverlayDefeats(NewVal);
//	}
//}
//void ABlasterPlayerController::SetHUDOverlayAmmo(int32 NewVal) {
//	if (ABlasterHUD* BlasterHUD = GetHUD<ABlasterHUD>()) {
//		BlasterHUD->SetOverlayAmmo(NewVal);
//	}
//}