//
// (C) Copyright 2024 Dirk Michael
// dirkmichaelnm@gmail.com
//

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "HexWorldGameModeBase.generated.h"

/**
 * Custom game mode base class for this game. 
 */
UCLASS()
class HEXWORLD_API AHexWorldGameModeBase : public AGameModeBase
{
	GENERATED_BODY()

	/**
	 * Default constructor
	 */
	AHexWorldGameModeBase();
};
