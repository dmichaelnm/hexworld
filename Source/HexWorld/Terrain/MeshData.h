//
// (C) Copyright 2024 Dirk Michael
// dirkmichaelnm@gmail.com
//

#pragma once

#include "CoreMinimal.h"
#include "IntVectorTypes.h"
#include "TerrainSize.h"
#include "MeshData.generated.h"

/**
 * This struct contains the necessary data to build a mesh.
 */
USTRUCT()
struct FMeshData
{
	GENERATED_BODY()

	/**
	 * A map between vertices and their indices. 
	 */
	TMap<UE::Geometry::FVector3i, int32> VertexMap;

	/**
	 * Array containing the raw vertices before they are distorted.
	 */
	TArray<FVector> RawVertexArray;
	
	/**
	 * Array containing the vertices of the mesh.
	 */
	TArray<FVector> VertexArray;

	/**
	 * Array containing the vertex indicies defining the triangles of the mesh.
	 */
	TArray<int32> TriangleArray;

	/**
	 * Struct with information about the size of the terrain.
	 */
	FTerrainSize TerrainSize;
};
