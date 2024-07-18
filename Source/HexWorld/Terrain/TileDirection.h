#pragma once

#include "CoreMinimal.h"

/**
 * This enumeration defines the six directions of tile where a neighbour tile can be.
 */
UENUM()
enum ETileDirection
{
	TopRight = 0,
	Right = 1,
	BottomRight = 2,
	BottomLeft = 3,
	Left = 4,
	TopLeft = 5
};
