// Copyright Na9i Studio.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameMode.h"
#include "BlasterGameMode.generated.h"

class ABlasterCharacter;
class ABlasterPlayerController;

UCLASS()
class BLASTER_API ABlasterGameMode : public AGameMode
{
	GENERATED_BODY()

public:
	ABlasterGameMode();

	virtual void Tick(float DeltaTime) override;

	virtual void EliminatePlayer(ABlasterCharacter* VictimPlayer, AController* VictimController, AController* AttackerController);

	void RequestRespawn(ABlasterCharacter* VictimPlayer, AController* VictimController);

	/* Match State */
	//void ClientPlayerReceiveMatchState(ABlasterPlayerController* BlasterPC);
	FORCEINLINE float GetMatchStateStartTime() const { return MatchStateStartTime; }
	float GetMatchStateTimeSeconds() const;

protected:
	virtual void BeginPlay() override;

	virtual void OnMatchStateSet() override;

	/* Match State */
	float MatchStateStartTime;

	UPROPERTY(EditDefaultsOnly)
	float WaitingToStartTimeSeconds;
	UPROPERTY(EditDefaultsOnly)
	float InProgressTimeSeconds;
	UPROPERTY(EditDefaultsOnly)
	float WaitingPostMatchTimeSeconds;
};