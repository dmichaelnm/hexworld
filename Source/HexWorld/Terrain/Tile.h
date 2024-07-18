#pragma once

#include "CoreMinimal.h"
#include "TilePosition.h"
#include "Tile.generated.h"

/**
 * This struct contains information about a terrain tile.
 */
USTRUCT()
struct FTile
{
	GENERATED_BODY()

	/**
	 * Default constructor.
	 */
	FTile()
	{
		Position = FTilePosition();	
	}

	/**
	 * Creates a new instance of a tile.
	 * 
	 * @param InPosition The position of the tile. 
	 */
	explicit FTile(const FTilePosition InPosition) {
		Position = InPosition;
	}
	
	/**
	 * The position of the terrain tile.
	 */
	FTilePosition Position;
};
