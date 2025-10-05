// Copyright Na9i Studio.


#include "Controller/BlasterPlayerController.h"
#include "PlayerState/BlasterPlayerState.h"
#include "Character/BlasterCharacter.h"
#include "UI/HUD/BlasterHUD.h"

#include "Blaster/Nani/NaniUtility.h"

ABlasterPlayerController::ABlasterPlayerController() :
	ServerClientDeltaTime{ 0.f },
	TimeCounter{ 0.f },
	ServerClientDeltaTimeUpdateFrequency{ 5.f }
{

}

void ABlasterPlayerController::Tick(float DeltaTime) {

	if (BlasterHUD) { /* which means we are local */
		SetHUDOverlayText(EOverlayText::EOT_CountDown, GetServerTime());

		/* Server-Client RoundTrip Time */
		TimeCounter += DeltaTime;
		if (TimeCounter > ServerClientDeltaTimeUpdateFrequency) {
			TimeCounter = 0.f;
			ServerRequestServerTime(GetWorld()->GetTimeSeconds());
		}
	}
}

void ABlasterPlayerController::OnPossess(APawn* aPawn) {
	Super::OnPossess(aPawn);

	NANI_LOG(Error, "OnPossess");

	/* needs local check 
	 * because this function is called on authority */

	if (IsLocalController()) {
		GetPlayerState<ABlasterPlayerState>()->SetReferences();
	}
}

void ABlasterPlayerController::OnRep_PlayerState() {
	Super::OnRep_PlayerState();

	NANI_LOG(Error, "OnRep_PlayerState");

	/* no local check worry
	 * because this function is only called locally
	 * since only locally controlled clients have a controller */

	/* not called when pawn respawns, since controller presists and has its own reference of playerstate */

	GetPlayerState<ABlasterPlayerState>()->SetReferences();
}

//
//============================================ HUD ============================================
//
void ABlasterPlayerController::UpdateHUDCrosshair(FCrosshairTextures& NewCrosshair, float NewRate, FLinearColor TintColor) {
	BlasterHUD = BlasterHUD ? BlasterHUD : GetHUD<ABlasterHUD>(); /* since we don't know when to set this shitz */

	if (BlasterHUD) BlasterHUD->UpdateCrosshair(NewCrosshair, NewRate, TintColor);
}

//
//============================================ HUD's Overlay ============================================
//
void ABlasterPlayerController::SetHUDOverlayText(EOverlayText TextWidget, float Value) {
	BlasterHUD = BlasterHUD ? BlasterHUD : GetHUD<ABlasterHUD>(); /* since we don't know when to set this shitz */

	if (BlasterHUD) BlasterHUD->SetOverlayText(TextWidget, Value);
}

//
//============================================ Server-Client RoundTrip Time ============================================
//
void ABlasterPlayerController::ServerRequestServerTime_Implementation(float ClientTime) {
	float ServerTime = GetWorld()->GetTimeSeconds();
	ClientReceiveServerTime(ClientTime, ServerTime);
}
void ABlasterPlayerController::ClientReceiveServerTime_Implementation(float ClientTime, float ServerTime) {
	float RoundTripTime = GetWorld()->GetTimeSeconds() - ClientTime;
	float CurrentServerTime = ServerTime + (0.5f * RoundTripTime);

	/* server client delta time is an offset value added to clients to sync up to server time */
	ServerClientDeltaTime = CurrentServerTime - GetWorld()->GetTimeSeconds();
}

float ABlasterPlayerController::GetServerTime() {
	return GetWorld()->GetTimeSeconds() + ServerClientDeltaTime;
	//if (HasAuthority()) {
	//	return GetWorld()->GetTimeSeconds();
	//}
	//else {
	//	/* since non anuthoritative controller is local controller */
	//	return GetWorld()->GetTimeSeconds() + ServerClientDeltaTime;
	//}
}

//
//============================================ Match State ============================================
//
void ABlasterPlayerController::ClientOnMatchStateSet_Implementation(FName State) {
	/* also called on authority local controller */
}