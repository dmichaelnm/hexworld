//
// (C) Copyright 2024 Dirk Michael
// dirkmichaelnm@gmail.com
//

#include "HexWorldGameModeBase.h"

#include "Terrain/TerrainCameraController.h"
#include "Terrain/TerrainCameraPawn.h"

/**
 * Default constructor.
 */
AHexWorldGameModeBase::AHexWorldGameModeBase()
{
	// Initialize custom classes
	DefaultPawnClass = ATerrainCameraPawn::StaticClass();
	PlayerControllerClass = ATerrainCameraController::StaticClass();
}
