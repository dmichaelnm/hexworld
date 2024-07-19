#pragma once

#include "CoreMinimal.h"
#include "NoiseParameter.generated.h"

/**
 * This struct contains parameter for calculating a noise value.
 */
USTRUCT()
struct FNoiseParameter
{
	GENERATED_BODY()

	/**
	 * Size vector.
	 */
	UPROPERTY(EditAnywhere, Category="Terrain Properties|Distortion")
	FVector2D Size;
	
	/**
	 * Offset vector.
	 */
	UPROPERTY(EditAnywhere, Category="Terrain Properties|Distortion")
	FVector2D Offset;

	/**
	 * Octaves.
	 */
	UPROPERTY(EditAnywhere, Category="Terrain Properties|Distortion")
	int32 Octaves;
	
	/**
	 * The noise frequency. 
	 */
	UPROPERTY(EditAnywhere, Category="Terrain Properties|Distortion")
	double Frequency;

	/**
	 * The noise amplitude.
	 */
	UPROPERTY(EditAnywhere, Category="Terrain Properties|Distortion")
	double Amplitude;

	/**
	 * The noise redistribution.
	 */
	UPROPERTY(EditAnywhere, Category="Terrain Properties|Distortion")
	double Redistribution;
};
