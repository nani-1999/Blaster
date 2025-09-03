// Copyright Na9i Studio.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
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
	void SetHUDOverlayHealth(float CurrentHealth, float MaxHealth);
};
