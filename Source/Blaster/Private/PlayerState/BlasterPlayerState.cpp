// Copyright Na9i Studio.


#include "PlayerState/BlasterPlayerState.h"
#include "Controller/BlasterPlayerController.h"
#include "Net/UnrealNetwork.h"
#include "UI/BlasterUITypes.h"

#include "Blaster/Nani/NaniUtility.h"

ABlasterPlayerState::ABlasterPlayerState() :
	//Elims{ 0 },
	Defeats{ 0 }
{

}

void ABlasterPlayerState::GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const {
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	//DOREPLIFETIME(ABlasterPlayerState, Elims);
	DOREPLIFETIME(ABlasterPlayerState, Defeats);
}

//
//============================================ Score ============================================
//
void ABlasterPlayerState::AddScore(float AddAmount) {
	/* Happens On Authority */
	float CurrentScore = GetScore(); /* accessing Score directly is prohibited, even though it is accessible */
	SetScore(CurrentScore + AddAmount);

	/* HUD's Overlay 
	 * since the owner of a APlayerState is AController */
	ABlasterPlayerController* BlasterPC = Cast<ABlasterPlayerController>(GetOwner());
	if (BlasterPC && BlasterPC->IsLocalController()) {
		BlasterPC->SetHUDOverlayText(EOverlayText::EOT_Score, GetScore());
	}
}
void ABlasterPlayerState::OnRep_Score() {
	Super::OnRep_Score();

	/* HUD's Overlay
	 * All clients has its own player state and get its reps notify 
	 * but only one client has viewport means owning-client which has a hud 
	 * so we update our hud there only */
	ABlasterPlayerController* BlasterPC = Cast<ABlasterPlayerController>(GetOwner());
	if (BlasterPC && BlasterPC->IsLocalController()) {
		BlasterPC->SetHUDOverlayText(EOverlayText::EOT_Score, GetScore());
	}
}

//
//============================================ Defeats ============================================
//
void ABlasterPlayerState::AddDefeats(int32 AddAmount) {
	/* Happens on Authority */
	Defeats += AddAmount;

	/* HUD's Overlay */
	ABlasterPlayerController* BlasterPC = Cast<ABlasterPlayerController>(GetOwner());
	if (BlasterPC && BlasterPC->IsLocalController()) {
		BlasterPC->SetHUDOverlayText(EOverlayText::EOT_Defeats, Defeats);
	}
}
void ABlasterPlayerState::OnRep_Defeats() {
	/* HUD's Overlay */
	ABlasterPlayerController* BlasterPC = Cast<ABlasterPlayerController>(GetOwner());
	if (BlasterPC && BlasterPC->IsLocalController()) {
		BlasterPC->SetHUDOverlayText(EOverlayText::EOT_Defeats, Defeats);
	}
}