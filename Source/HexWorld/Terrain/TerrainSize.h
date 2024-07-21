#pragma once

#include "CoreMinimal.h"
#include "TerrainSize.generated.h"

/**
 * This struct contains information about the size of the terrain.
 */
USTRUCT()
struct FTerrainSize
{
	GENERATED_BODY()

	/**
	 * The minimal X coordinate.
	 */
	double MinimalX;

	/**
	 * The minimal Y coordinate.
	 */
	double MinimalY;

	/**
	 * The minimal X coordinate.
	 */
	double MaximalX;

	/**
	 * The minimal Y coordinate.
	 */
	double MaximalY;
};
