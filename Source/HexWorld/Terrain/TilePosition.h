#pragma once

#include "CoreMinimal.h"
#include "TilePosition.generated.h"

/**
 * This struct contains the coordinates of the position of a terrain tile.
 */
USTRUCT()
struct FTilePosition
{
	GENERATED_BODY()

	/**
	 * Default constructor.
	 */
	FTilePosition()
	{
		X = 0;
		Y = 0;
		Z = 0;
	}

	/**
	 * Creates a new tile position instance.
	 * 
	 * @param InX X coordinate. 
	 * @param InY Y coordinate.
	 * @param InZ Z coordinate.
	 */
	explicit FTilePosition(const int32 InX, const int32 InY, const int32 InZ)
	{
		X = InX;
		Y = InY;
		Z = InZ;
	}
	
	/**
	 * The X coordinate of a terrain tile
	 */
	int32 X;

	/**
	 * The Y coordinate of a terrain tile
	 */
	int32 Y;

	/**
	 * The Z coordinate of a terrain tile
	 */
	int32 Z;
};
