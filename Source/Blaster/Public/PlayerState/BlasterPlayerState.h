// Copyright Na9i Studio.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "BlasterPlayerState.generated.h"

UCLASS()
class BLASTER_API ABlasterPlayerState : public APlayerState
{
	GENERATED_BODY()

public:
	ABlasterPlayerState();

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

protected:
	/* Score */
	virtual void OnRep_Score() override;

	/* Elims */
	//UPROPERTY(ReplicatedUsing = OnRep_Elims)
	//int32 Elims;
	//UFUNCTION()
	//virtual void OnRep_Elims();

	/* Defeats */
	UPROPERTY(ReplicatedUsing = OnRep_Defeats)
	int32 Defeats;
	UFUNCTION()
	virtual void OnRep_Defeats();

public:
	void AddScore(float AddAmount);
	//void AddElims(int32 AddAmount);
	void AddDefeats(int32 AddAmount);
	FORCEINLINE int32 GetDefeats() const { return Defeats; }
};
