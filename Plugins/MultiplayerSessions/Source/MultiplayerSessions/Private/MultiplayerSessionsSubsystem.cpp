// Fill out your copyright notice in the Description page of Project Settings.


#include "MultiplayerSessionsSubsystem.h"
#include "OnlineSubsystem.h"
#include "OnlineSubsystemSteam.h"
#include "OnlineSessionSettings.h"

UMultiplayerSessionsSubsystem::UMultiplayerSessionsSubsystem() :
	OnCreateSessionComplete_Delegate{ FOnCreateSessionCompleteDelegate::CreateUObject(this, &UMultiplayerSessionsSubsystem::CreateSessionComplete_Callback) },
	OnFindSessionsComplete_Delegate{ FOnFindSessionsCompleteDelegate::CreateUObject(this, &UMultiplayerSessionsSubsystem::FindSessionsComplete_Callback) },
	OnJoinSessionComplete_Delegate{ FOnJoinSessionCompleteDelegate::CreateUObject(this, &UMultiplayerSessionsSubsystem::JoinSessionComplete_Callback) },
	OnStartSessionComplete_Delegate{ FOnStartSessionCompleteDelegate::CreateUObject(this, &UMultiplayerSessionsSubsystem::StartSessionComplete_Callback) },
	OnDestroySessionComplete_Delegate{ FOnDestroySessionCompleteDelegate::CreateUObject(this, &UMultiplayerSessionsSubsystem::DestroySessionComplete_Callback) }
{
	/* IOnlineSubsystem */
	IOnlineSubsystem* OnlineSubsystemInterface = IOnlineSubsystem::Get();
	if (OnlineSubsystemInterface) {
		{
			// @DEBUG
			if (GEngine) GEngine->AddOnScreenDebugMessage(0, 20.f, FColor::Blue, FString::Printf(TEXT("Subsystem Found: %s"), *OnlineSubsystemInterface->GetSubsystemName().ToString())); /* NULL is UE default OnlineSubsystem */
		}

		/* IOnlineSession */
		OnlineSessionInterface = OnlineSubsystemInterface->GetSessionInterface();
	}
}

bool UMultiplayerSessionsSubsystem::CreateGameSession(int32 NumPublicConnections, FString MatchType, FString LobbyPath) {
	checkf(OnlineSessionInterface, TEXT("OnlineSessionInterface is Invalid"));

	/* lastly traveled lobby path */
	LastLobbyPathAsListen = LobbyPath + FString("?listen");

	return	CreateSession(NumPublicConnections, MatchType);
}
bool UMultiplayerSessionsSubsystem::FindGameSessions(int32 MaxSearchResults) {
	checkf(OnlineSessionInterface, TEXT("OnlineSessionInterface is Invalid"));

	return FindSessions(MaxSearchResults);
}
bool UMultiplayerSessionsSubsystem::JoinGameSession() {
	checkf(OnlineSessionInterface, TEXT("OnlineSessionInterface is Invalid"));

	/* joining first found session with a MatchType */
	for (FOnlineSessionSearchResult& SessionSearchResult : LastSessionSearch->SearchResults) {
		FString SessionID = SessionSearchResult.Session.GetSessionIdStr(); /* session id */
		FString SessionUser = SessionSearchResult.Session.OwningUserName; /* session user, steam user name */
		FString SessionMatchType; /* session type */
		SessionSearchResult.Session.SessionSettings.Get(FName("MatchType"), SessionMatchType);

		if (SessionMatchType.Equals(FString("FreeForAll"))) {
			return JoinSession(SessionSearchResult);
		}
	}

	return false;
}

//
// ============================================== Destroy Session ==================================================
//
bool UMultiplayerSessionsSubsystem::DestroySession(FName SessionName) {
	checkf(OnlineSessionInterface, TEXT("OnlineSessionInterface is Invalid"));
	
	/* adding out delegate to delegate list, before destorying
	 * delegate will broadcast, after session is destroyed
	 * make sure to remove the delegate from delegate list on callback */
	DestroySessionComplete_DelegateHandle = OnlineSessionInterface->AddOnDestroySessionCompleteDelegate_Handle(OnDestroySessionComplete_Delegate);

	/* destroying session if already existed */
	bool bDestroySuccessful = OnlineSessionInterface->DestroySession(NAME_GameSession);
	if (!bDestroySuccessful) {
		OnlineSessionInterface->ClearOnDestroySessionCompleteDelegate_Handle(DestroySessionComplete_DelegateHandle);
	}

	return bDestroySuccessful;
}
void UMultiplayerSessionsSubsystem::DestroySessionComplete_Callback(FName SessionName, bool bWasSuccessful) {

	{
		// @DEBUG
		if (GEngine) GEngine->AddOnScreenDebugMessage(11, 20.f, bWasSuccessful ? FColor::Green : FColor::Red, bWasSuccessful ? FString::Printf(TEXT("Destroy Session Successful : %s"), *SessionName.ToString()) : FString::Printf(TEXT("Failed To Destroy Session : %s"), *SessionName.ToString()));
	}

	/* we remove delegate from the delegate list irrespective of the success */
	OnlineSessionInterface->ClearOnDestroySessionCompleteDelegate_Handle(DestroySessionComplete_DelegateHandle);
}

//
// ============================================== Create Session ==================================================
//
bool UMultiplayerSessionsSubsystem::CreateSession(int32 NumPublicConnections, FString MatchType) {
	checkf(OnlineSessionInterface, TEXT("OnlineSessionInterface is Invalid"));

	/* getting session with the default name */
	FNamedOnlineSession* ExistingSession = OnlineSessionInterface->GetNamedSession(NAME_GameSession);
	if (ExistingSession) {
		if (GEngine) GEngine->AddOnScreenDebugMessage(11, 20.f, FColor::Red, FString("%s Session Already Exist. Destroying  Session ..."));
		DestroySession(NAME_GameSession);
		return false;
	}

	/* adding our delegate to delegate list, before creating session
	 * delegate will broadcast, after session is created */
	CreateSessionComplete_DelegateHandle = OnlineSessionInterface->AddOnCreateSessionCompleteDelegate_Handle(OnCreateSessionComplete_Delegate);

	/* session settings */
	TSharedPtr<FOnlineSessionSettings> NewSessionSettings = MakeShareable(new FOnlineSessionSettings()); /* since smart-pointers auto free, based on reference count */
	NewSessionSettings->bIsLANMatch = false; /* not a local network, connecting to internet */
	NewSessionSettings->NumPublicConnections = NumPublicConnections; /* num of player that can connect to this session */
	NewSessionSettings->bAllowJoinInProgress = true; /* join while session is already started */
	NewSessionSettings->bAllowJoinViaPresence = true; /* join session through region aka steam region */
	NewSessionSettings->bShouldAdvertise = true; /* enabling steam to advertise session, so that others can join through steam */
	NewSessionSettings->bUsesPresence = true; /* marking this session as a region */
	NewSessionSettings->bUseLobbiesIfAvailable = true; /*  */
	NewSessionSettings->Set(FName("MatchType"), MatchType, EOnlineDataAdvertisementType::ViaOnlineServiceAndPing); /* session type, user defined */
	NewSessionSettings->BuildUniqueId = 1; /* filters out other unknown players sessions */

	const ULocalPlayer* LocalPlayer = GetGameInstance()->GetFirstGamePlayer();
	/* creating session
	 * if we failed to create session, we remove delegate from the delegate list
	 * just in case if delegate wasn't fired */
	bool bCreateSuccessful = OnlineSessionInterface->CreateSession(*LocalPlayer->GetPreferredUniqueNetId(), NAME_GameSession, *NewSessionSettings);
	if (!bCreateSuccessful) {
		OnlineSessionInterface->ClearOnCreateSessionCompleteDelegate_Handle(CreateSessionComplete_DelegateHandle);
	}

	return bCreateSuccessful;
}
void UMultiplayerSessionsSubsystem::CreateSessionComplete_Callback(FName SessionName, bool bWasSuccessful) {
	checkf(OnlineSessionInterface, TEXT("OnlineSessionInterface is Invalid"));

	{
		// @DEBUG
		if (GEngine) GEngine->AddOnScreenDebugMessage(2, 20.f, bWasSuccessful ? FColor::Green : FColor::Red, bWasSuccessful ? FString::Printf(TEXT("Create Session Successful : %s"), *SessionName.ToString()) : FString::Printf(TEXT("Failed To Create Session : %s"), *SessionName.ToString()));
	}

	/* we remove delegate from the delegate list irrespective of the success */
	OnlineSessionInterface->ClearOnCreateSessionCompleteDelegate_Handle(CreateSessionComplete_DelegateHandle);

	/* server travel
	 * we only server travel if we successfully created a session */
	if (bWasSuccessful) {
		if (UWorld* World = GetWorld()) {
			World->ServerTravel(LastLobbyPathAsListen);
		}
	}
}

//
// ============================================== Find Session ==================================================
//
bool UMultiplayerSessionsSubsystem::FindSessions(int32 MaxSearchResults) {
	checkf(OnlineSessionInterface, TEXT("OnlineSessionInterface is Invalid"));

	/* adding delegate to delegate list, before find sessions
	 * delegate will broadcast, after find sessions is completed */
	FindSessionsComplete_DelegateHandle = OnlineSessionInterface->AddOnFindSessionsCompleteDelegate_Handle(OnFindSessionsComplete_Delegate);

	/* session search
	 * member variabling, so we can access search results anywhere
	 * filled with search results after finding sessions */
	LastSessionSearch = MakeShareable(new FOnlineSessionSearch());
	LastSessionSearch->MaxSearchResults = MaxSearchResults; /* large number because there are a lot of sessions make by other developers on that steam app id */
	LastSessionSearch->bIsLanQuery = false; /* obviously not using lan */
	LastSessionSearch->QuerySettings.Set(SEARCH_PRESENCE, true, EOnlineComparisonOp::Equals); /* since we are using presence. including sessions which are using presence */

	ULocalPlayer* LocalPlayer = GetGameInstance()->GetFirstGamePlayer();
	/* finding sessions 
	 * this will find sessions and fill the search search with results */
	bool bFindSuccessful = OnlineSessionInterface->FindSessions(*LocalPlayer->GetPreferredUniqueNetId(), LastSessionSearch.ToSharedRef());
	if (!bFindSuccessful) {
		OnlineSessionInterface->ClearOnFindSessionsCompleteDelegate_Handle(FindSessionsComplete_DelegateHandle);
	}

	return bFindSuccessful;
}
void UMultiplayerSessionsSubsystem::FindSessionsComplete_Callback(bool bWasSuccessful) {
	checkf(OnlineSessionInterface, TEXT("OnlineSessionInterface is Invalid"));

	/* we remove delegate from the delegate list irrespective of the success */
	OnlineSessionInterface->ClearOnFindSessionsCompleteDelegate_Handle(FindSessionsComplete_DelegateHandle);

	/* printing search results on screen */
	TArray<FOnlineSessionSearchResult>& SessionSearchResults = LastSessionSearch->SearchResults;
	{
		// @DEBUG
		if (GEngine) GEngine->AddOnScreenDebugMessage(4, 20.f, FColor::Yellow, FString::Printf(TEXT("Sessions Found: %d"), SessionSearchResults.Num()));
	}
	for (FOnlineSessionSearchResult& SessionSearchResult : SessionSearchResults) {
		FString SessionID = SessionSearchResult.Session.GetSessionIdStr(); /* session id */
		FString SessionUser = SessionSearchResult.Session.OwningUserName; /* session user, steam user name */
		FString SessionMatchType; /* session type */
		SessionSearchResult.Session.SessionSettings.Get(FName("MatchType"), SessionMatchType);
		{
			// @DEBUG
			if (GEngine) GEngine->AddOnScreenDebugMessage(5, 20.f, FColor::Yellow, FString::Printf(TEXT("Session ID : %s | Session User : %s | Session Match Type : %s"), *SessionID, *SessionUser, *SessionMatchType));
		}
	}
}

//
// ============================================== Join Session ==================================================
//
bool UMultiplayerSessionsSubsystem::JoinSession(FOnlineSessionSearchResult& SearchResult) {
	checkf(OnlineSessionInterface, TEXT("OnlineSessionInterface is Invalid"));

	/* adding delegate to delegate list, before find sessions
	 * delegate will broadcast, after join sessions is completed */
	JoinSessionComplete_DelegateHandle = OnlineSessionInterface->AddOnJoinSessionCompleteDelegate_Handle(OnJoinSessionComplete_Delegate);

	ULocalPlayer* LocalPlayer = GetGameInstance()->GetFirstGamePlayer();
	/* joining session 
	 * joining to the given session search result */
	bool bJoinSuccessful = OnlineSessionInterface->JoinSession(*LocalPlayer->GetPreferredUniqueNetId(), NAME_GameSession, SearchResult);
	if (!bJoinSuccessful) {
		OnlineSessionInterface->ClearOnJoinSessionCompleteDelegate_Handle(JoinSessionComplete_DelegateHandle);
	}

	return bJoinSuccessful;
}
void UMultiplayerSessionsSubsystem::JoinSessionComplete_Callback(FName SessionName, EOnJoinSessionCompleteResult::Type JoinResult) {
	checkf(OnlineSessionInterface, TEXT("OnlineSessionInterface is Invalid"));

	{
		// @DEBUG
		FString JoinResultStr = FString("JoinSession Completed : ");
		FColor JoinResultClr = FColor::Red;
		switch (JoinResult) {
			case EOnJoinSessionCompleteResult::AlreadyInSession :
				JoinResultStr += FString("Already In Session");
				JoinResultClr = FColor::Green;
				break;
			case EOnJoinSessionCompleteResult::CouldNotRetrieveAddress :
				JoinResultStr += FString("Could Not Receive Address");
				break;
			case EOnJoinSessionCompleteResult::SessionDoesNotExist :
				JoinResultStr += FString("Session Does Not Exist");
				break;
			case EOnJoinSessionCompleteResult::SessionIsFull :
				JoinResultStr += FString("Session Is Full");
				break;
			case EOnJoinSessionCompleteResult::Success :
				JoinResultStr += FString("Success");
				JoinResultClr = FColor::Green;
				break;
			default:
				JoinResultStr += FString("Unknown Error");
		}
		if (GEngine) GEngine->AddOnScreenDebugMessage(7, 20.f, JoinResultClr, FString::Printf(TEXT("Join Session %s Completed with Result %s"), *SessionName.ToString(), *JoinResultStr));
	}
	
	/* we remove delegate from the delegate list irrespective of the success */
	OnlineSessionInterface->ClearOnJoinSessionCompleteDelegate_Handle(JoinSessionComplete_DelegateHandle);

	/* getting the address of current joined session */
	FString SessionAddress;
	OnlineSessionInterface->GetResolvedConnectString(SessionName, SessionAddress);

	APlayerController* PlayerController = GetGameInstance()->GetFirstLocalPlayerController();
	/* client travel 
	 * we only client travel if we successfully joined a session */
	PlayerController->ClientTravel(SessionAddress, ETravelType::TRAVEL_Absolute);
}

//
// ============================================== Start Session ==================================================
//
void UMultiplayerSessionsSubsystem::StartSession() {

}
void UMultiplayerSessionsSubsystem::StartSessionComplete_Callback(FName SessionName, bool bWasSuccessful) {

}

