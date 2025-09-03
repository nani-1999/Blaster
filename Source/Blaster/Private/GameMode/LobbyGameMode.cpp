// Copyright Na9i Studio.


#include "GameMode/LobbyGameMode.h"
#include "GameFramework/GameStateBase.h"
#include "GameFramework/PlayerState.h"

#include "Blaster/Nani/NaniUtility.h"

void ALobbyGameMode::PostLogin(APlayerController* NewPlayer) {
	Super::PostLogin(NewPlayer);

	if (GameState) {
		int32 NumberOfPlayers = GameState->PlayerArray.Num();
		GEngine->AddOnScreenDebugMessage(11, 20.f, FColor::Orange, FString::Printf(TEXT("Number Of Players: %d"), NumberOfPlayers));

		APlayerState* NewPlayerState = NewPlayer->GetPlayerState<APlayerState>();
		FName SessionName = NewPlayerState->SessionName;
		FString PlayerName = NewPlayerState->GetPlayerName();
	
		GEngine->AddOnScreenDebugMessage(12, 20.f, FColor::Orange, FString::Printf(TEXT("%s has Entered the %s"), *PlayerName, *SessionName.ToString()));

		/* Seemless Travel */
		// @DEBUG
		if (GEngine) GEngine->AddOnScreenDebugMessage(21, 20.f, FColor::Black, FString::Printf(TEXT("%s | Server Travel"), *GetNetModeStr<FString>(GetWorld()->GetNetMode())));
		if (NumberOfPlayers >= 2) {
			bUseSeamlessTravel = true;
			GetWorld()->ServerTravel(FString("/Game/Levels/OpenWorldLevel?listen"));
		}
	}
}

void ALobbyGameMode::Logout(AController* Exiting) {
	Super::Logout(Exiting);

	if (GameState) {
		int32 NumberOfPlayers = GameState->PlayerArray.Num();
		GEngine->AddOnScreenDebugMessage(11, 20.f, FColor::Orange, FString::Printf(TEXT("Number Of Players: %d"), NumberOfPlayers));
	}

	APlayerState* ExitingPlayerState = Exiting->GetPlayerState<APlayerState>();
	FName SessionName = ExitingPlayerState->SessionName;
	FString PlayerName = ExitingPlayerState->GetPlayerName();

	GEngine->AddOnScreenDebugMessage(12, 20.f, FColor::Orange, FString::Printf(TEXT("%s has Entered the %s"), *PlayerName, *SessionName.ToString()));
}
