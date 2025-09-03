// Copyright Na9i Studio.


#include "GameMode/BlasterGameMode.h"
#include "Character/BlasterCharacter.h"

#include "Blaster/Nani/NaniUtility.h"

void ABlasterGameMode::EliminatePlayer(ABlasterCharacter* VictimPlayer, AController* VictimController, AController* AttackerController) {
	NANI_LOG(Warning, "%s Eliminated %s", *AttackerController->GetName(), *VictimPlayer->GetName());
	VictimPlayer->MulticastEliminated();
}
