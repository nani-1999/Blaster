// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "Interfaces/OnlineSessionInterface.h"
#include "MultiplayerSessionsSubsystem.generated.h"

/**
 * 
 */
UCLASS()
class MULTIPLAYERSESSIONS_API UMultiplayerSessionsSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()
	
public:
	UMultiplayerSessionsSubsystem();

	bool DestroySession(FName SessionName);
	bool CreateSession(int32 NumPublicConnections, FString MatchType); 
	bool FindSessions(int32 MaxSearchResults);
	bool JoinSession(FOnlineSessionSearchResult& SearchResult);
	void StartSession();

	/* host session */
	bool CreateGameSession(int32 NumPublicConnections, FString MatchType, FString LobbyPath);
	/* find session */
	bool FindGameSessions(int32 MaxSearchResults);
	/* join session */
	bool JoinGameSession();

protected:
	/* Session Settings */
	TSharedPtr<FOnlineSessionSettings> SessionSettings;

	/* Create Session */
	FOnCreateSessionCompleteDelegate OnCreateSessionComplete_Delegate;
	void CreateSessionComplete_Callback(FName SessionName, bool bWasSuccessful);
	FDelegateHandle CreateSessionComplete_DelegateHandle;

	FString LastLobbyPathAsListen;

	/* Find Sessions */
	FOnFindSessionsCompleteDelegate OnFindSessionsComplete_Delegate;
	void FindSessionsComplete_Callback(bool bWasSuccessful);
	FDelegateHandle FindSessionsComplete_DelegateHandle;

	TSharedPtr<FOnlineSessionSearch> LastSessionSearch;

	/* Join Session */
	FOnJoinSessionCompleteDelegate OnJoinSessionComplete_Delegate;
	void JoinSessionComplete_Callback(FName SessionName, EOnJoinSessionCompleteResult::Type JoinResult);
	FDelegateHandle JoinSessionComplete_DelegateHandle;

	/* Start Session */
	FOnStartSessionCompleteDelegate OnStartSessionComplete_Delegate;
	void StartSessionComplete_Callback(FName SessionName, bool bWasSuccessful);
	FDelegateHandle StartSessionComplete_DelegateHandle;

	/* Destroy Session */
	FOnDestroySessionCompleteDelegate OnDestroySessionComplete_Delegate;
	void DestroySessionComplete_Callback(FName SessionName, bool bWasSuccessful);
	FDelegateHandle DestroySessionComplete_DelegateHandle;

private:
	IOnlineSessionPtr OnlineSessionInterface;
};
