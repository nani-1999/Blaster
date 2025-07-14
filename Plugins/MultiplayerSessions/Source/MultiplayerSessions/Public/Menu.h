// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Menu.generated.h"

class UButton;
class UMultiplayerSessionsSubsystem;

UCLASS()
class MULTIPLAYERSESSIONS_API UMenu : public UUserWidget
{
	GENERATED_BODY()

protected:
	virtual void NativeOnInitialized() override;

	/* Button */
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> Host_Btn;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> Find_Btn;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> Join_Btn;

	/* Button Callbacks */
	UFUNCTION()
	void Host_Btn_Callback();
	UFUNCTION()
	void Find_Btn_Callback();
	UFUNCTION()
	void Join_Btn_Callback();

	/* Multiplayer Session Subsystem */
	UMultiplayerSessionsSubsystem* MultiplayerSessionsSubsystem;

	/* Create Session */
	int32 NumOfPublicConnections;
	FString MatchTypeStr;
	FString LobbyPathStr;
	/* Find Session */
	int32 MaxSearchResults;

public:
	UFUNCTION(BlueprintCallable)
	void SetupMenu(int32 NumberOfPublicConnections = 4, FString MatchType = FString(TEXT("FreeForAll")), FString LobbyPath = FString(TEXT("LobbyPath")), int32 MaximumSearchResults = 10000);
};
