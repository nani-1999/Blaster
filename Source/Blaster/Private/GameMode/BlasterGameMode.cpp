// Copyright Na9i Studio.


#include "GameMode/BlasterGameMode.h"
#include "Character/BlasterCharacter.h"
#include "PlayerState/BlasterPlayerState.h"
#include "Controller/BlasterPlayerController.h"

#include "Blaster/Nani/NaniUtility.h"


ABlasterGameMode::ABlasterGameMode() :
	//GameModeStartingTime{ 0.f },
	WarmupTime{ 10.f },
	CountdownTime{ 0.f }
{
	/* this will prevent match to auto start */
	bDelayedStart = true;

	/* the entirity of game mode happens on authority */
}

void ABlasterGameMode::BeginPlay() {
	Super::BeginPlay();

	/* time when this game mode starts 
	 * since GetTimeSeconds() is game specific, we offset it by a value when the game mode starts */
	//GameModeStartingTime = GetWorld()->GetTimeSeconds();

	CountdownTime = WarmupTime;
}

void ABlasterGameMode::Tick(float DeltaTime) {
	Super::Tick(DeltaTime);

	/* jumping into game after some cooldown */
	if (MatchState == MatchState::WaitingToStart) {
		//CountdownTime = WarmupTime - GetWorld()->GetTimeSeconds() + GameModeStartingTime; /* just decrementing countdowntime instead */
		CountdownTime -= DeltaTime;
		if (CountdownTime <= 0.f) {
			StartMatch();
		}
	}
}

void ABlasterGameMode::OnMatchStateSet() {
	Super::OnMatchStateSet();

	/* we are doing rpc instead of making a property replication on player controller */

	/* a iterator which consists of all playercontrollers */
	for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It) {
		if (ABlasterPlayerController* BlasterPC = Cast<ABlasterPlayerController>(*It)) {
			BlasterPC->ClientOnMatchStateSet(MatchState);
		}
	}
}

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
