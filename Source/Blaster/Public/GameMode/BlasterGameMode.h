// Copyright Na9i Studio.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameMode.h"
#include "BlasterGameMode.generated.h"

class ABlasterCharacter;

UCLASS()
class BLASTER_API ABlasterGameMode : public AGameMode
{
	GENERATED_BODY()

public:
	ABlasterGameMode();

	virtual void Tick(float DeltaTime) override;

	virtual void EliminatePlayer(ABlasterCharacter* VictimPlayer, AController* VictimController, AController* AttackerController);

	void RequestRespawn(ABlasterCharacter* VictimPlayer, AController* VictimController);

protected:
	//float GameModeStartingTime;
	float WarmupTime;
	float CountdownTime;

	virtual void BeginPlay() override;

	virtual void OnMatchStateSet() override;
};