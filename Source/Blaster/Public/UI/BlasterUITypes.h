#pragma once

//#include "CoreMinimal.h"
#include "BlasterUITypes.generated.h" 

/* since we use GENERATED_BODY macro in struct, we might include .generated.h
 * GENERATED_BODY() (along with the USTRUCT() macro) allows Unreal Header Tool (UHT) 
 * to generate the necessary code for the struct 
 * to be integrated into Unreal's reflection system. 
 * This means the engine can understand the struct's properties, functions, and other metadata at runtime */

USTRUCT()
struct FCrosshairTextures
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<UTexture2D> Center;
	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<UTexture2D> Top;
	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<UTexture2D> Right;
	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<UTexture2D> Bottom;
	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<UTexture2D> Left;
};

enum class EOverlayText : unsigned char {
	EOT_MaxHealth,
	EOT_Health,
	EOT_Score,
	EOT_Elims,
	EOT_Defeats,
	EOT_Ammo,
	//EOT_AmmoCapacity,
	EOT_CarriedAmmo,
	EOT_MAX
};