//
// (C) Copyright 2024 Dirk Michael
// dirkmichaelnm@gmail.com
//

#pragma once

#include "CoreMinimal.h"
#include "MeshData.h"
#include "NoiseParameter.h"
#include "GameFramework/Actor.h"
#include "ProceduralMeshComponent.h"
#include "Tile.h"
#include "TileDirection.h"
#include "TerrainActor.generated.h"

// Defines the log category of this class.
DECLARE_LOG_CATEGORY_EXTERN(TerrainActor, Log, All);

UCLASS()
class HEXWORLD_API ATerrainActor : public AActor
{
	GENERATED_BODY()

public:
	/**
	 * Default constructor.
	 */
	ATerrainActor();

protected:
	/**
	 * Called when the game starts or when spawned.
	 */
	virtual void BeginPlay() override;

public:
	/**
	 * Called every frame
	 */
	virtual void Tick(const float DeltaTime) override;

protected:
	/**
	 * The procedural mesh component containing the terrain and water. This component will be the root component of the
	 * actor.
	 */
	UPROPERTY(VisibleAnywhere, Category = "Terrain")
	UProceduralMeshComponent* MeshComponent;

	/**
	 * The height that defines the sea level.
	 */
	UPROPERTY(EditAnywhere, Category = "Terrain Properties")
	int32 SeaLevel;

	/**
	 * The material to be applied to the terrain mesh.
	 */
	UPROPERTY(EditAnywhere, Category = "Terrain Properties|Materials")
	UMaterialInterface* TerrainMaterial;

	/**
	 * The material to be applied to the water mesh.
	 */
	UPROPERTY(EditAnywhere, Category = "Terrain Properties|Materials")
	UMaterialInterface* WaterMaterial;

	/**
	 * A texture containing informationen about the topography of the terrain.
	 */
	UPROPERTY(EditAnywhere, Category = "Terrain Properties|Topography Map")
	UTexture2D* Topography;

	/**
	 * The factor used to calculate the height of a tile from the red color value of a topograohy texture pixel.
	 * Z = Red / HeightFactor.
	 */
	UPROPERTY(EditAnywhere, Category = "Terrain Properties|Topography Map")
	int32 HeightFactor;

	/**
	 * The amount of the height unit. 
	 */
	UPROPERTY(EditAnywhere, Category = "Terrain Properties|Mesh")
	double HeightUnit;

	/**
	 * The height of the upper and lower edge of a wall
	 * (specified in part in height units, should be between 0.0 and 1.0).
	 */
	UPROPERTY(EditAnywhere, Category = "Terrain Properties|Mesh")
	double WallEdgeHeight;

	/**
	 * The offset height of the water mesh.
	 */
	UPROPERTY(EditAnywhere, Category = "Terrain Properties|Mesh")
	double WaterOffset;

	/**
	 * The scale amount for the mesh vertices.
	 */
	UPROPERTY(EditAnywhere, Category = "Terrain Properties|Mesh")
	double Scale;

	/**
	 * Noise parameter for the X axis.
	 */
	UPROPERTY(EditAnywhere, Category = "Terrain Properties|Distortion")
	FNoiseParameter NoiseParameterX;

	/**
	 * Noise parameter for the Y axis.
	 */
	UPROPERTY(EditAnywhere, Category = "Terrain Properties|Distortion")
	FNoiseParameter NoiseParameterY;

	/**
	 * Noise parameter for the Z axis.
	 */
	UPROPERTY(EditAnywhere, Category = "Terrain Properties|Distortion")
	FNoiseParameter NoiseParameterZ;

public:
	/**
	 * Removes all generated meshes.
	 */
	UFUNCTION(CallInEditor, Category = "Terrain Properties")
	void Clear() const;

	/**
	 * Builds the meshes for the terrain and the water.
	 */
	UFUNCTION(CallInEditor, Category = "Terrain Properties")
	void Build();

private:
	// Attributes

	/**
	 * The width of the terrain counted in tiles. 
	 */
	int32 SizeX;

	/**
	 * The length of the terrain counted in tiles. 
	 */
	int32 SizeY;

	/**
	 * Array of terrain tiles. The index of the tile defines the position in the map.
	 * Index = X + Y * SizeX
	 */
	TArray<FTile> Tiles;

	// Methods

	/**
	 * Reads the terrain data from the topography texture and initializes the tiles array.
	 */
	void ReadTopography();

	/**
	 * Creates a mesh based on the specified mesh data.
	 *
	 * @param Section The index of the mesh section.
	 * @param MeshData The mesh data struct.
	 * @param Material The material to be applied to the mesh.
	 */
	void BuildMesh(const int32 Section, const FMeshData& MeshData, UMaterialInterface* Material) const;

	/**
	 * Generates the mesh data for the water mesh.
	 * 
	 * @return Mesh data struct. 
	 */
	FMeshData GenerateWaterMeshData() const;

	/**
	 * Generates the mesh data for the terrain mesh.
	 * 
	 * @return Mesh data struct. 
	 */
	FMeshData GenerateTerrainMeshData() const;

	/**
	 * Generates the mesh data for the center part of the tile mesh.
	 * 
	 * @param MeshData The mesh data struct. 
	 * @param Tile The tile the mesh data is generated for.
	 */
	void GenerateTerrainTileCenter(FMeshData& MeshData, const FTile& Tile) const;

	/**
	 * Generates the mesh data for the inner edge in the specified direction for the specified tile.
	 * 
	 * @param MeshData The mesh data struct. 
	 * @param Tile The tile the mesh data is generated for.
	 * @param Direction The direction of the inner edge.
	 * @param CenterZ The height of the neighbour in the specified direction.
	 */
	void GenerateTerrainTileInnerEdge(FMeshData& MeshData, const FTile& Tile, const ETileDirection Direction,
	                                  const int32 CenterZ) const;

	/**
	 * Generates the mesh data for the outer edge in the specified direction for the specified tile.
	 * 
	 * @param MeshData The mesh data struct. 
	 * @param Tile The tile the mesh data is generated for.
	 * @param Direction The direction of the outer edge.
	 * @param LeftZ The height of the neighbour of the left side of the specified direction.
	 * @param CenterZ The height of the neighbour in the specified direction.
	 * @param RightZ The height of the neighbour of the right side of the specified direction.
	 */
	void GenerateTerrainTileOuterEdge(FMeshData& MeshData, const FTile& Tile, const ETileDirection Direction,
	                                  const int32 LeftZ, const int32 CenterZ, const int32 RightZ) const;

	/**
	 * Generates the mesh data for the left and right inner corner in the specified direction for the specified tile.
	 * 
	 * @param MeshData The mesh data struct. 
	 * @param Tile The tile the mesh data is generated for.
	 * @param Direction The direction of the inner corner.
	 * @param LeftZ The height of the neighbour of the left side of the specified direction.
	 * @param CenterZ The height of the neighbour in the specified direction.
	 * @param RightZ The height of the neighbour of the right side of the specified direction.
	 */
	void GenerateTerrainTileInnerCorners(FMeshData& MeshData, const FTile& Tile, const ETileDirection Direction,
	                                     const int32 LeftZ, const int32 CenterZ, const int32 RightZ) const;

	/**
	 * Generates the mesh data for the left and right outer corner in the specified direction for the specified tile.
	 * 
	 * @param MeshData The mesh data struct. 
	 * @param Tile The tile the mesh data is generated for.
	 * @param Direction The direction of the outer corner.
	 * @param LeftZ The height of the neighbour of the left side of the specified direction.
	 * @param CenterZ The height of the neighbour in the specified direction.
	 * @param RightZ The height of the neighbour of the right side of the specified direction.
	 */
	void GenerateTerrainTileOuterCorners(FMeshData& MeshData, const FTile& Tile, const ETileDirection Direction,
	                                     const int32 LeftZ, const int32 CenterZ, const int32 RightZ) const;

	/**
	 * Generates the mesh data for the center wall in the specified direction for the specified tile.
	 * 
	 * @param MeshData The mesh data struct. 
	 * @param Tile The tile the mesh data is generated for.
	 * @param Direction The direction of the center wall.
	 * @param CenterZ The height of the neighbour in the specified direction.
	 */
	void GenerateTerrainTileCenterWall(FMeshData& MeshData, const FTile& Tile, const ETileDirection Direction,
	                                   const int32 CenterZ) const;

	/**
	 * Generates the mesh data for the left or right side wall in the specified direction for the specified tile.
	 * 
	 * @param MeshData The mesh data struct. 
	 * @param Tile The tile the mesh data is generated for.
	 * @param Direction The direction of the side wall.
	 * @param CenterZ The height of the neighbour in the specified direction.
	 * @param SideZ The height of the neighbour of the specified side of the specified direction.
	 * @param Index0 The index of the upper vertex. 
	 * @param Index1 The index of the lower vertex.
	 */
	void GenerateTerrainTileSideWall(FMeshData& MeshData, const FTile& Tile, const ETileDirection Direction,
	                                 const int32 CenterZ, const int32 SideZ, const int32 Index0,
	                                 const int32 Index1) const;

	/**
	 * Generates the mesh data for the left side corner wall in the specified direction for the specified tile.
	 * 
	 * @param MeshData The mesh data struct. 
	 * @param Tile The tile the mesh data is generated for.
	 * @param Direction The direction of the left side corner wall.
	 * @param CenterZ The height of the neighbour in the specified direction.
	 * @param LeftZ The height of the neighbour of the left side of the specified direction.
	 */
	void GenerateTerrainTileLeftCornerWall(FMeshData& MeshData, const FTile& Tile, const ETileDirection Direction,
	                                       const int32 CenterZ, const int32 LeftZ) const;

	/**
	 * Generates the mesh data for the right side corner wall in the specified direction for the specified tile.
	 * 
	 * @param MeshData The mesh data struct. 
	 * @param Tile The tile the mesh data is generated for.
	 * @param Direction The direction of the right side corner wall.
	 * @param CenterZ The height of the neighbour in the specified direction.
	 * @param RightZ The height of the neighbour of the right side of the specified direction.
	 */
	void GenerateTerrainTileRightCornerWall(FMeshData& MeshData, const FTile& Tile, const ETileDirection Direction,
	                                        const int32 CenterZ, const int32 RightZ) const;

	/**
	 * Adds a new vertex to the current or a new triangle in the mesh data.
	 * 
	 * @param MeshData The mesh data struct. 
	 * @param Tile The tile the vertex is generated for.
	 * @param Index The index of the vertex position within a tile.
	 * @param Height The height of the vertex in height units. If <i>Absolute</i> is <b>true</b>,
	 *               the height is used as specified and not in height units.
	 * @param Absolute If <b>true</b>, the height is used as is and is not multiplied with the height unit.
	 * @param NoDistortion If <b>true</b>, the vertext will be distorted, otherwise not.
	 */
	void AddVertex(FMeshData& MeshData, const FTile& Tile, const int32 Index, const double Height,
	               const bool Absolute = false, const bool NoDistortion = false) const;

	/**
	 * Adds a new triangle to the mesh data struct. The vertices are calculated by the specified direction and the
	 * local vertex indicies and heights of all threee vertices of the triangle.
	 * 
	 * @param MeshData The mesh data struct. 
	 * @param Tile The tile the triangle is generated for.
	 * @param Direction The direction of the part of the tile mesh.
	 * @param Index0 The first local vertex index.
	 * @param Height0 The height of the first local vertex.
	 * @param Index1 The second local vertex index.
	 * @param Height1 The height of the second local vertex.
	 * @param Index2 The third local vertex index.
	 * @param Height2 The height of the third local vertex.
	 */
	void AddTriangle(FMeshData& MeshData, const FTile& Tile, const ETileDirection Direction, const int32 Index0,
	                 const double Height0, const int32 Index1, const double Height1, const int32 Index2,
	                 const double Height2) const;

	/**
	 * Returns an array with three items that are the heights (Z coordinates) of the neighbour tiles.
	 * Index 0 : The height of the neighbour of the left side of the specified direction.
	 * Index 1 : The height of the neighbour of the specified direction.
	 * Index 2 : The height of the neighbour of the right side of the specified direction.
	 * 
	 * @param Tile The tile for which the neighbour heights are determined. 
	 * @param Direction The direction in which the neighbours are.
	 * 
	 * @return Array with three integer values for the heights of the neighbour tiles. 
	 */
	TArray<int32> GetNeighbourHeights(const FTile& Tile, const ETileDirection Direction) const;

	/**
	 * Returns a pointer to the neighbour tile in the specified direction for the specified tile. If there is no tile
	 * in that direction, a <i>nullptr</i> is returned.
	 * 
	 * @param Tile The tile for which the neighbour is determined.
	 * @param Direction The direction of the neighbour tile.
	 * 
	 * @return A pointer to the neighbour tile or <i>nullptr</i>. 
	 */
	const FTile* GetNeighbour(const FTile& Tile, const ETileDirection Direction) const;

	/**
	 * Returns a pointer to the tile at the specified coordinates. If the coordinates are invalid, a <i>nullptr</i> is
	 * returned.
	 * 
	 * @param X The X coordinate of the tile. 
	 * @param Y The Y coordinate of the tile.
	 * 
	 * @return A pointer to the tile or a <i>nullptr</i>. 
	 */
	const FTile* GetTile(const int32 X, const int32 Y) const;

	/**
	 * Checks if there is water in the specified direction for the specified tile.
	 * 
	 * @param Tile The tile to be checked for having a coast.
	 * @param Direction The direction that is checked.
	 * 
	 * @return If there is water in the specified direction then <b>true</b>, otherwise <b>false</b>. 
	 */
	bool HasCoast(const FTile& Tile, const ETileDirection Direction) const;

	/**
	 * Checks if there is water in any direction of the specified tile.
	 * 
	 * @param Tile The tile to be checked for having a coast.
	 * 
	 * @return If there is water in at least one direction then <b>true</b>, otherwise <b>false</b>. 
	 */
	bool HasCoast(const FTile& Tile) const;

	/**
	 * Calculate the heights of the vertices for the left or right inner corner of the tile mesh. An array with four
	 * items containing the heights of the four vertices is returned.
	 * 
	 * @param Tile The tile the heights are calculated for.
	 * @param CenterZ The height of the neighbour in the specified direction.
	 * @param SideZ The height of the neighbour of the specified side of the specified direction.
	 * 
	 * @return Array with four double values. 
	 */
	static TArray<double> CalculateInnerCornerHeights(const FTile& Tile, const int32 CenterZ, const int32 SideZ);

	/**
	 * Calculate the heights of the vertices for the left or right outer corner of the tile mesh. An array with four
	 * items containing the heights of the four vertices is returned.
	 * 
	 * @param Tile The tile the heights are calculated for.
	 * @param CenterZ The height of the neighbour in the specified direction.
	 * @param SideZ The height of the neighbour of the specified side of the specified direction.
	 * 
	 * @return Array with four double values. 
	 */
	static TArray<double> CalculateOuterCornerHeights(const FTile& Tile, const int32 CenterZ, const int32 SideZ);

	/**
	 * Calculates the array of the UV coordinates for the specified mesh data.
	 * 
	 * @param MeshData The mesh data struct.
	 * 
	 * @return Array of 2D vectors. 
	 */
	static TArray<FVector2D> CalculateUVArray(const FMeshData& MeshData);

	/**
	 * Calculates the array of the normal vectors for the specified mesh data.
	 * 
	 * @param MeshData The mesh data struct.
	 * 
	 * @return Array of vectors. 
	 */
	static TArray<FVector> CalculateNormalArray(const FMeshData& MeshData);

	/**
	 * Calculates a noise value for the specified coordinates and the noise parameter.
	 * 
	 * @param Px X coordinate. 
	 * @param Py Y coordinate.
	 * @param Params Noise parameter.
	 * 
	 * @return The noise value. 
	 */
	static double Noise(const double Px, const double Py, const FNoiseParameter& Params);

	/**
	 * Calculates a noise vector based on the specified vertex and the noise parameter for each axis.
	 * 
	 * @param Vertex Original vertex.
	 * @param ParamsX Noise parameter for X axis.
	 * @param ParamsY Noise parameter for Y axis.
	 * @param ParamsZ Noise parameter for Z axis.
	 * 
	 * @return The noise vector. 
	 */
	static FVector Noise(const FVector& Vertex, const FNoiseParameter& ParamsX, const FNoiseParameter& ParamsY,
	                     const FNoiseParameter& ParamsZ);
};
