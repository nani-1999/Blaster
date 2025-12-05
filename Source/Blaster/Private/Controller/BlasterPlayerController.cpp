// Copyright Na9i Studio.


#include "Controller/BlasterPlayerController.h"
#include "PlayerState/BlasterPlayerState.h"
//#include "Character/BlasterCharacter.h"
#include "UI/HUD/BlasterHUD.h"
#include "GameMode/BlasterGameMode.h"

#include "Blaster/Nani/NaniUtility.h"

ABlasterPlayerController::ABlasterPlayerController() :
	TimeCounter{ 5.f },
	ServerClientDeltaSecondsUpdateFrequency{ 5.f },
	ServerClientDeltaSeconds{ 0.f },

	GM_MatchStateStartTime{ 0.f },
	GM_MatchStateTimeSeconds{ 0.f }
{

}

void ABlasterPlayerController::BeginPlay() {
	Super::BeginPlay();

	NANI_LOG(Error, "%s | BeginPlay", *GetName());

	/* GetHUD() returns valid pointer only by local controller */
	BlasterHUD = BlasterHUD ? BlasterHUD : GetHUD<ABlasterHUD>();
	if (BlasterHUD) {
		BlasterHUD->CreateOverlay(this);
		BlasterHUD->CreateCountdownText(this);
		BlasterHUD->CreateAnnouncementOverlay(this);

		/* Match State 
		 * initially we need to know the state of the game */
		ServerRequestMatchState();
	}
}

void ABlasterPlayerController::Tick(float DeltaTime) {
	Super::Tick(DeltaTime);

	if (BlasterHUD) LocalTick(DeltaTime);
}
void ABlasterPlayerController::LocalTick(float DeltaTime) {
	/* Server-Client RoundTrip Time */
	TimeCounter += DeltaTime;
	if (TimeCounter > ServerClientDeltaSecondsUpdateFrequency) {
		TimeCounter = 0.f;
		ServerRequestServerTime(GetWorld()->GetTimeSeconds());
	}

	/* HUD's Announcement */
	float CurrentServerTime = GetServerTime();
	BlasterHUD->SetCountdownText(CurrentServerTime);
	BlasterHUD->SetAnnouncementCountdown(GM_MatchStateTimeSeconds - (CurrentServerTime - GM_MatchStateStartTime)); /* countdown = 1 - countup */
}

//void ABlasterPlayerController::OnPossess(APawn* aPawn) {
//	Super::OnPossess(aPawn);
//
//	NANI_LOG(Error, "OnPossess");
//
//	/* needs local check 
//	 * because this function is called on authority */
//
//	if (IsLocalController()) {
//		GetPlayerState<ABlasterPlayerState>()->SetReferences();
//	}
//}

void ABlasterPlayerController::InitPlayerState() {
	Super::InitPlayerState();

	/* Happens on Authority */

	NANI_LOG(Error, "%s | InitPlayerState", *GetName());

	if (IsLocalController()) {
		GetPlayerState<ABlasterPlayerState>()->SetReferences();
	}
}

void ABlasterPlayerController::OnRep_PlayerState() {
	Super::OnRep_PlayerState();

	NANI_LOG(Error, "%s | OnRep_PlayerState", *GetName());

	/* no local check worry
	 * because this function is only called locally
	 * since only locally controlled clients have a controller */

	/* not called when pawn respawns, since on client controller presists and its own reference of playerstate too */

	GetPlayerState<ABlasterPlayerState>()->SetReferences();
}

//
//============================================ HUD ============================================
//
void ABlasterPlayerController::UpdateHUDCrosshair(FCrosshairTextures& NewCrosshair, float NewRate, FLinearColor TintColor) {
	/* this check because, in server and client order of exection is different 
	 * eg. in server char::possessedby(PS's PC is null at this time) before ctrl::beginplay or other ctrl functions
	 * but in client ctrl::beginplay called first 
	 * so always check */
	//BlasterHUD = BlasterHUD ? BlasterHUD : GetHUD<ABlasterHUD>();
	BlasterHUD->UpdateCrosshair(NewCrosshair, NewRate, TintColor);
}
//
//============================================ HUD's Overlay ============================================
//
void ABlasterPlayerController::SetHUDOverlayVisibility(bool bVisible) {
	//BlasterHUD = BlasterHUD ? BlasterHUD : GetHUD<ABlasterHUD>();
	BlasterHUD->SetOverlayVisibility(bVisible);
}
void ABlasterPlayerController::SetHUDOverlayText(EOverlayText TextWidget, float Value) {
	//BlasterHUD = BlasterHUD ? BlasterHUD : GetHUD<ABlasterHUD>();
	BlasterHUD->SetOverlayText(TextWidget, Value);
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

	/* server client delta seconds, is just an offset value added to clients to sync up to server time
	 * instead of getting ServerTime everytime, we get it one time and calculate difference, so that we can offset it by our time whenever we need */
	ServerClientDeltaSeconds = CurrentServerTime - GetWorld()->GetTimeSeconds();
}

float ABlasterPlayerController::GetServerTime() {
	return GetWorld()->GetTimeSeconds() + ServerClientDeltaSeconds;
}

//
//============================================ Match State ============================================
//
void ABlasterPlayerController::ServerRequestMatchState_Implementation() {
	/* Happens on Authority */
	if (ABlasterGameMode* BlasterGM = GetWorld()->GetAuthGameMode<ABlasterGameMode>()) {
		ClientOnMatchStateSet(BlasterGM->GetMatchState(), BlasterGM->GetMatchStateStartTime(), BlasterGM->GetMatchStateTimeSeconds());
	}
}
void ABlasterPlayerController::ClientOnMatchStateSet_Implementation(FName State, float StateStartTime, float StateTimeSeconds) {
	NANI_LOG(Warning, "%s | ClientOnMatchStateSet: %s", *GetName(), *State.ToString());

	/* if we already in that state we skip */
	if (GM_MatchState == State) return;

	/* means we are in different state now */
	GM_MatchState = State;
	GM_MatchStateStartTime = StateStartTime;
	GM_MatchStateTimeSeconds = StateTimeSeconds;

	/* HUD's Announcement Overlay */
	/* hud doesn't need to know FName of MatchState, but enum of MatchSTate which is EBlasterMatchState 
	 * we skip other MatchState */
	//BlasterHUD = BlasterHUD ? BlasterHUD : GetHUD<ABlasterHUD>();
	if (BlasterHUD) {
		if (GM_MatchState == MatchState::WaitingToStart) {
			BlasterHUD->SetAnnouncementMatchState(EAnnouncementMatchState::EAMS_WaitingToStart);
		}
		else if (GM_MatchState == MatchState::InProgress) {
			BlasterHUD->SetAnnouncementMatchState(EAnnouncementMatchState::EAMS_InProgress);
		}
		else if (GM_MatchState == MatchState::WaitingPostMatch) {
			BlasterHUD->SetAnnouncementMatchState(EAnnouncementMatchState::EAMS_WaitingPostMatch);

			/* disabling player's controller-privileges */
			if (APawn* CtrlPawn = GetPawn()) CtrlPawn->DisableInput(this);
		}
		else /*if (GM_MatchState == MatchState::LeavingMap)*/ {
			/* this is custom */
			BlasterHUD->SetAnnouncementOverlayVisibility(false);
			BlasterHUD->SetOverlayVisibility(false);
		}
	}
}