// Copyright Na9i Studio.


#include "PlayerState/BlasterPlayerState.h"
#include "Controller/BlasterPlayerController.h"
#include "Net/UnrealNetwork.h"
#include "UI/BlasterUITypes.h"

#include "Blaster/Nani/NaniUtility.h"

ABlasterPlayerState::ABlasterPlayerState() :
	Defeats{ 0 }
{

}

void ABlasterPlayerState::GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const {
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	//DOREPLIFETIME(ABlasterPlayerState, Elims);
	DOREPLIFETIME(ABlasterPlayerState, Defeats);
}

//
//============================================ References ============================================
//
void ABlasterPlayerState::SetReferences() {
	/* this func is called on locally only */
	BlasterPC = GetOwner<ABlasterPlayerController>();
}
void ABlasterPlayerState::InitOverlay() {
	/* HUD's Overlay */
	BlasterPC->SetHUDOverlayText(EOverlayText::EOT_Score, GetScore());
	BlasterPC->SetHUDOverlayText(EOverlayText::EOT_Defeats, Defeats);
}

//
//============================================ Score ============================================
//
void ABlasterPlayerState::AddScore(float AddAmount) {
	/* Happens On Authority */
	float CurrentScore = GetScore(); /* accessing Score directly is prohibited, even though it is accessible */
	SetScore(CurrentScore + AddAmount);

	/* HUD's Overlay */
	if (BlasterPC) BlasterPC->SetHUDOverlayText(EOverlayText::EOT_Score, GetScore());
}
void ABlasterPlayerState::OnRep_Score() {
	Super::OnRep_Score();

	/* HUD's Overlay*/
	if (BlasterPC) BlasterPC->SetHUDOverlayText(EOverlayText::EOT_Score, GetScore());
}

//
//============================================ Defeats ============================================
//
void ABlasterPlayerState::AddDefeats(int32 AddAmount) {
	/* Happens on Authority */
	Defeats += AddAmount;

	/* HUD's Overlay */
	if (BlasterPC) BlasterPC->SetHUDOverlayText(EOverlayText::EOT_Defeats, Defeats);
}
void ABlasterPlayerState::OnRep_Defeats() {
	/* HUD's Overlay */
	if (BlasterPC) BlasterPC->SetHUDOverlayText(EOverlayText::EOT_Defeats, Defeats);
}