// Copyright Na9i Studio.


#include "GameMode/BlasterGameMode.h"
#include "Character/BlasterCharacter.h"
#include "PlayerState/BlasterPlayerState.h"

#include "Blaster/Nani/NaniUtility.h"

void ABlasterGameMode::EliminatePlayer(ABlasterCharacter* VictimPlayer, AController* VictimController, AController* AttackerController) {
	NANI_LOG(Warning, "%s Eliminated %s", *AttackerController->GetName(), *VictimPlayer->GetName());

	/* adding score on attacker's player state 
	 * checking if attacker and victim player state doesn't match, since we die from our own nades 
	 * but we do add the defeats tho, irrespective of whatever */
	ABlasterPlayerState* AttackerPS = AttackerController->GetPlayerState<ABlasterPlayerState>();
	ABlasterPlayerState* VictimPS = VictimController->GetPlayerState<ABlasterPlayerState>();
	if (AttackerPS && AttackerPS != VictimPS) {
		AttackerPS->AddScore(1.f);
	}
	if (VictimPS) VictimPS->AddDefeats(1);
	
	/* eliminating victim player */
	VictimPlayer->Eliminated();
}

void ABlasterGameMode::RequestRespawn(ABlasterCharacter* VictimPlayer, AController* VictimController) {
	if (VictimPlayer) {
		VictimPlayer->Reset();
		VictimPlayer->Destroy();
	}
	if (VictimController) {
		RestartPlayer(VictimController);
	}
}