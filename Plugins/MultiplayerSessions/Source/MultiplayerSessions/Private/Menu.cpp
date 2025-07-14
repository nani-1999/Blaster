// Fill out your copyright notice in the Description page of Project Settings.


#include "Menu.h"
#include "Components/Button.h"
#include "MultiplayerSessionsSubsystem.h"

void UMenu::NativeOnInitialized() {
	Super::NativeOnInitialized();

	/* native functions are for widget things only */

	/* Binding Callbacks */
	if (Host_Btn) Host_Btn->OnClicked.AddDynamic(this, &UMenu::Host_Btn_Callback);
	if (Find_Btn) Find_Btn->OnClicked.AddDynamic(this, &UMenu::Find_Btn_Callback);
	if (Join_Btn) Join_Btn->OnClicked.AddDynamic(this, &UMenu::Join_Btn_Callback);
}

void UMenu::SetupMenu(int32 NumberOfPublicConnections, FString MatchType, FString LobbyPath, int32 MaximumSearchResults) {

	NumOfPublicConnections = NumberOfPublicConnections;
	MatchTypeStr = MatchType;
	LobbyPathStr = LobbyPath;
	MaxSearchResults = MaximumSearchResults;

	UGameInstance* GameInstance = GetGameInstance();
	if (GameInstance) {

		/* getting Mutliplayer Sessions Subsystem */
		MultiplayerSessionsSubsystem = GameInstance->GetSubsystem<UMultiplayerSessionsSubsystem>();

		/* Cursor
	     * always do cursors with first controller */
		APlayerController* PlayerController = GameInstance->GetFirstLocalPlayerController();
		if (PlayerController) {

			/* enabling input to ui only */
			FInputModeUIOnly InputModeData; /* derived from FInputModeDataBase */
			InputModeData.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock); /** cursor lock to viewport behavior */
			InputModeData.SetWidgetToFocus(TakeWidget());

			PlayerController->SetInputMode(InputModeData); /* Input(mouse clicks) Only Effects on UI */
			PlayerController->SetShowMouseCursor(true);
		}

		/* adding widget to viewport */
		AddToViewport();
		SetVisibility(ESlateVisibility::Visible);
		SetIsFocusable(true);
	}
}

void UMenu::Host_Btn_Callback() {
	// @DEBUG
	if (GEngine) GEngine->AddOnScreenDebugMessage(1, 20.f, FColor::Purple, FString("HostButtonClicked"));

	Host_Btn->SetIsEnabled(false);

	if (MultiplayerSessionsSubsystem) {
		/* hosting session */
		bool bHostingSuccessful = MultiplayerSessionsSubsystem->CreateGameSession(NumOfPublicConnections, MatchTypeStr, LobbyPathStr);

		/* enabling input back to game only */
		if (bHostingSuccessful) {
			if (UGameInstance* GameInstance = GetGameInstance()) {
				APlayerController* PlayerController = GameInstance->GetFirstLocalPlayerController();
				if (PlayerController) {
					FInputModeGameOnly InputModeData;
					PlayerController->SetInputMode(InputModeData);
					PlayerController->SetShowMouseCursor(false);
				}
			}
		}
		else {
			Host_Btn->SetIsEnabled(true);
		}
	}

}
void UMenu::Find_Btn_Callback() {
	if (GEngine) GEngine->AddOnScreenDebugMessage(3, 20.f, FColor::Purple, FString("FindButtonClicked"));

	if (MultiplayerSessionsSubsystem) {
		bool bFindSuccessful = MultiplayerSessionsSubsystem->FindGameSessions(MaxSearchResults);
	}
}
void UMenu::Join_Btn_Callback() {
	if (GEngine) GEngine->AddOnScreenDebugMessage(6, 20.f, FColor::Purple, FString("JoinButtonClicked"));

	Join_Btn->SetIsEnabled(false);

	if (MultiplayerSessionsSubsystem) {
		/* joining session */
		bool bJoiningSuccessful = MultiplayerSessionsSubsystem->JoinGameSession();

		/* enabling input back to game only */
		if (bJoiningSuccessful) {
			if (UGameInstance* GameInstance = GetGameInstance()) {
				APlayerController* PlayerController = GameInstance->GetFirstLocalPlayerController();
				if (PlayerController) {
					FInputModeGameOnly InputModeData;
					PlayerController->SetInputMode(InputModeData);
					PlayerController->SetShowMouseCursor(false);
				}
			}
		}
		else {
			Host_Btn->SetIsEnabled(true);
		}
	}
}
