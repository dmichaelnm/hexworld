#pragma once

#include "CoreMinimal.h"
#include "IntVectorTypes.h"
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
	 * Array containing the vertices of the mesh.
	 */
	TArray<FVector> VertexArray;

	/**
	 * Array containing the vertex indicies defining the triangles of the mesh.
	 */
	TArray<int32> TriangleArray;

	/**
	 * The smallest X coordinate of all vertices. 
	 */
	double MinimalX;

	/**
	 * The greatest X coordinate of all vertices.
	 */
	double MaximalX;
	
	/**
	 * The smallest Y coordinate of all vertices. 
	 */
	double MinimalY;
	
	/**
	 * The greatest Y coordinate of all vertices.
	 */
	double MaximalY;
};
