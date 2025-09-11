// Copyright Na9i Studio.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "UI/BlasterUITypes.h"
#include "BlasterPlayerController.generated.h"

/**
 * 
 */
UCLASS()
class BLASTER_API ABlasterPlayerController : public APlayerController
{
	GENERATED_BODY()
	
public:
	/* HUD's Overlay */
	void SetupHUDOverlay();
	void SetHUDOverlayText(EOverlayText TextWidget, float Value);
	//void SetHUDOverlayMaxHealth(float NewVal);
	//void SetHUDOverlayHealth(float NewVal);
	//void SetHUDOverlayScore(float NewVal);
	//void SetHUDOverlayDefeats(int32 NewVal);
	//void SetHUDOverlayAmmo(int32 NewVal);
};
