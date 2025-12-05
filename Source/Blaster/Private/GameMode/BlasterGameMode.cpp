// Copyright Na9i Studio.


#include "GameMode/BlasterGameMode.h"
#include "Character/BlasterCharacter.h"
#include "PlayerState/BlasterPlayerState.h"
#include "Controller/BlasterPlayerController.h"

#include "Blaster/Nani/NaniUtility.h"


ABlasterGameMode::ABlasterGameMode() :
	MatchStateStartTime{ -1.f },

	WaitingToStartTimeSeconds{ 10.f },
	InProgressTimeSeconds{ 20.f },
	WaitingPostMatchTimeSeconds{ 10.f }
{
	/* this will prevent match to auto start */
	bDelayedStart = true;

	/* the entirity of game mode happens on authority */
}

void ABlasterGameMode::BeginPlay() {
	Super::BeginPlay();

	MatchStateStartTime = GetWorld()->GetTimeSeconds();
}

void ABlasterGameMode::Tick(float DeltaTime) {
	Super::Tick(DeltaTime);

	/* game mode is server
	 * so whenever ServerTravel()s that level's game mode initially has countdowntime before starting match 
	 * this is only occurred once to a traveled level */

	float MatchStateTimeSeconds = GetWorld()->GetTimeSeconds() - MatchStateStartTime; /* difference from Countdown Started Time to Current Time */

	if (MatchState == MatchState::WaitingToStart) {
		if (MatchStateTimeSeconds > WaitingToStartTimeSeconds) {
			StartMatch(); /* don't worry OnMatchStateSet() handles both MatchStateStartTime by Us & MatchState by Super */
		}
	}
	else if (MatchState == MatchState::InProgress) {
		if (MatchStateTimeSeconds > InProgressTimeSeconds) {
			EndMatch();
		}
	}
	else if (MatchState == MatchState::WaitingPostMatch) {
		if (MatchStateTimeSeconds > WaitingPostMatchTimeSeconds) {
			RestartGame(); /* re-enters/re-travels the same current level. all things reset, nothing presists */
		}
	}
}

//
//============================================ Match State ============================================
//
void ABlasterGameMode::OnMatchStateSet() {
	Super::OnMatchStateSet();

	NANI_LOG(Warning, "%s | OnMatchStateSet: %s", *GetName(), *MatchState.ToString());

	/* this is tick safe, means this func is called before tick(), in stack */
	/* any MatchState changes goes through this function */

	MatchStateStartTime = GetWorld()->GetTimeSeconds();

	/* a iterator which consists of all playercontrollers */
	/* we are doing rpc instead of making a property replication on player controller */
	for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It) {
		if (ABlasterPlayerController* BlasterPC = Cast<ABlasterPlayerController>(*It)) {
			BlasterPC->ClientOnMatchStateSet(MatchState, MatchStateStartTime, GetMatchStateTimeSeconds());
		}
	}
}
float ABlasterGameMode::GetMatchStateTimeSeconds() const {
	if (MatchState == MatchState::WaitingToStart) {
		return WaitingToStartTimeSeconds;
	}
	else if (MatchState == MatchState::InProgress) {
		return InProgressTimeSeconds;
	}
	else if (MatchState == MatchState::WaitingPostMatch) {
		return WaitingPostMatchTimeSeconds;
	}
	else { 
		return 0.f; 
	}
}

//
//============================================ Player Elimination and Respawn ============================================
//
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
















//if (WarmupBeginTime > 0.f) {
//	if ((CurrentGameTime - WarmupBeginTime) > WarmupTimeSeconds) {
//		WarmupBeginTime = -1.f; /* to block the entry */
//		MatchBeginTime = CurrentGameTime; /* to allow the entry of next check */

//		NANI_LOG(Warning, "MatchTime Being");
//	}
//}
//else if (MatchBeginTime > 0.f) {
//	if ((CurrentGameTime - MatchBeginTime) > MatchTimeSeconds) {
//		MatchBeginTime = -1.f;
//		//PostMatchBeginTime = CurrentGameTime;

//		NANI_LOG(Warning, "PostMatchTime Begin");
//	}

//}
//else if (PostMatchBeginTime > 0.f) {
//	if ((CurrentGameTime - PostMatchBeginTime) > PostMatchTimeSeconds) {
//		PostMatchBeginTime = -1.f;

//		NANI_LOG(Warning, "Showing ScoreBoard enough, Now Restarting the Game");

//		WarmupBeginTime = CurrentGameTime;
//	}
//}



//if (MatchState == MatchState::WaitingToStart) {
//	//CountdownTime = WarmupTime - GetWorld()->GetTimeSeconds() + GameModeStartingTime; /* just decrementing countdowntime instead */
//	
//	CountdownTime -= DeltaTime;
//	if (CountdownTime <= 0.f) {
//		StartMatch();
//	}
//}