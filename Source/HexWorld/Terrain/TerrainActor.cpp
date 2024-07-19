//
// (C) Copyright 2024 Dirk Michael
// dirkmichaelnm@gmail.com
//

#include "TerrainActor.h"

#include "IntVectorTypes.h"
#include "TileDirection.h"

// Defines the log category of this class.
DEFINE_LOG_CATEGORY(TerrainActor)
// Define the enum range for tile directions.
ENUM_RANGE_BY_FIRST_AND_LAST(ETileDirection, ETileDirection::TopRight, ETileDirection::TopLeft)

// The width of a tile
#define TILE_WIDTH sqrt(3.0) / 2.0
// The factor for creating a vertex key
#define KEY_FACTOR 1000000.0

/**
 * Defines the indicies of the vertices for each of the six parts of a hexagon tile.
 */
const int32 EdgeVertices[6][35] = {
	// Top Right
	{
		1600, 1536, 1472, 1408, 1344,
		1732, 1668, 1604, 1540, 1476, 1412,
		1864, 1800, 1736, 1672, 1608, 1544, 1480,
		1996, 1932, 1868, 1804, 1740, 1676, 1612, 1548,
		2128, 2064, 2000, 1936, 1872, 1808, 1744, 1680, 1616
	},
	// Right
	{
		1344, 1212, 1080, 948, 816,
		1412, 1280, 1148, 1016, 884, 752,
		1480, 1348, 1216, 1084, 952, 820, 688,
		1548, 1416, 1284, 1152, 1020, 888, 756, 624,
		1616, 1484, 1352, 1220, 1088, 956, 824, 692, 560
	},
	// Bottom Right
	{
		816, 748, 680, 612, 544,
		752, 684, 616, 548, 480, 412,
		688, 620, 552, 484, 416, 348, 280,
		624, 556, 488, 420, 352, 284, 216, 148,
		560, 492, 424, 356, 288, 220, 152, 84, 16
	},
	// Bottom Left
	{
		544, 608, 672, 736, 800,
		412, 476, 540, 604, 668, 732,
		280, 344, 408, 472, 536, 600, 664,
		148, 212, 276, 340, 404, 468, 532, 596,
		16, 80, 144, 208, 272, 336, 400, 464, 528
	},
	// Left
	{
		800, 932, 1064, 1196, 1328,
		732, 864, 996, 1128, 1260, 1392,
		664, 796, 928, 1060, 1192, 1324, 1456,
		596, 728, 860, 992, 1124, 1256, 1388, 1520,
		528, 660, 792, 924, 1056, 1188, 1320, 1452, 1584
	},
	// Top Left
	{
		1328, 1396, 1464, 1532, 1600,
		1392, 1460, 1528, 1596, 1664, 1732,
		1456, 1524, 1592, 1660, 1728, 1796, 1864,
		1520, 1588, 1656, 1724, 1792, 1860, 1928, 1996,
		1584, 1652, 1720, 1788, 1856, 1924, 1992, 2060, 2128
	}
};

/**
 * Default constructor.
 */
ATerrainActor::ATerrainActor()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	// Create procedural mesh component and set it as root component.
	MeshComponent = CreateDefaultSubobject<UProceduralMeshComponent>(TEXT("Procedural Mesh Component"));
	SetRootComponent(MeshComponent);

	// Initialize default values.
	SeaLevel = 0;
	HeightFactor = 8;
	HeightUnit = 0.025;
	WallEdgeHeight = 0.5;
	WaterOffset = 0.0;
	Scale = 100.0;
}

/**
 * Called when the game starts or when spawned.
 */
void ATerrainActor::BeginPlay()
{
	Super::BeginPlay();
}

/**
 * Called every frame
 */
void ATerrainActor::Tick(const float DeltaTime)
{
	Super::Tick(DeltaTime);
}

/**
 * Removes all generated meshes.
 */
void ATerrainActor::Clear() const
{
	MeshComponent->ClearMeshSection(0);
	MeshComponent->ClearMeshSection(1);
}

/**
 * Builds the meshes for the terrain and the water.
 */
void ATerrainActor::Build()
{
	// Log
	UE_LOG(TerrainActor, Display, TEXT("Start building terrain..."));

	// Clear the tiles array
	Tiles.Empty();
	// Check, if a topography texture is set
	if (IsValid(Topography))
	{
		// Load the topography of the terrain from the texture
		ReadTopography();
	}
	else
	{
		// TODO Procedural terrain generation 		
	}

	// Generate terrain mesh data.
	const auto TerrainMeshData = GenerateTerrainMeshData();
	// Generate dynamic terrain material
	const auto DynamicTerrainMaterial = UMaterialInstanceDynamic::Create(
		TerrainMaterial, nullptr, TEXT("Dynamic Terrain Material"));
	// Set scale parameter
	DynamicTerrainMaterial->SetScalarParameterValue(TEXT("Scale"), Scale);
	// Set grid tiling parameter
	DynamicTerrainMaterial->SetScalarParameterValue(TEXT("Grid Tile X"), SizeX + 0.5);
	DynamicTerrainMaterial->SetScalarParameterValue(TEXT("Grid Tile Y"), (SizeY * 0.75 + 0.25) / 1.5);
	// Build the terrain mesh
	BuildMesh(0, TerrainMeshData, DynamicTerrainMaterial);
	// Generate water mesh data
	const auto WaterMeshData = GenerateWaterMeshData();
	// Build the water mesh
	BuildMesh(1, WaterMeshData, WaterMaterial);
}

/**
 * Reads the terrain data from the topography texture and initializes the tiles array.
 */
void ATerrainActor::ReadTopography()
{
	// Apply properties to the topography texture.
	Topography->CompressionSettings = TC_VectorDisplacementmap;
	Topography->MipGenSettings = TMGS_NoMipmaps;
	Topography->Filter = TF_Nearest;
	Topography->SRGB = false;
	Topography->UpdateResource();
	// Get dimensions of the topography texture
	SizeX = Topography->GetSizeX();
	SizeY = Topography->GetSizeY();
	// Get and lock image data
	const auto Mip = &Topography->GetPlatformData()->Mips[0];
	auto RawImageData = Mip->BulkData;
	const auto ColorImageData = static_cast<FColor*>(RawImageData.Lock(LOCK_READ_ONLY));
	// Read the pixels of the texture and initialize the tiles array
	for (auto Y = 0; Y < SizeY; Y++)
	{
		for (auto X = 0; X < SizeX; X++)
		{
			// Get the color of the pixel
			const auto Color = ColorImageData[X + Y * SizeX];
			// The height of the tile is encoded in the red part of the color.
			const auto Z = Color.R / HeightFactor;
			// Add new tile to the tiles array
			Tiles.Add(FTile(FTilePosition(X, Y, Z - SeaLevel)));
		}
	}
	// Unlock the image data
	RawImageData.Unlock();

	// Log
	UE_LOG(TerrainActor, Display, TEXT("Topography read (%d x %d, %d tiles)."), SizeX, SizeY, Tiles.Num());
}

/**
 * Creates a mesh based on the specified mesh data.
 *
 * @param Section The index of the mesh section.
 * @param MeshData The mesh data struct.
 * @param Material The material to be applied to the mesh.
 */
void ATerrainActor::BuildMesh(const int32 Section, const FMeshData& MeshData, UMaterialInterface* Material) const
{
	// Get the vertex array
	const auto Vertices = MeshData.VertexArray;
	// Get the triangle array
	const auto Triangles = MeshData.TriangleArray;
	// Get the UV array
	const auto UVs = CalculateUVArray(MeshData);
	// Get the normal array
	const auto Normals = CalculateNormalArray(MeshData);

	// Create the mesh
	MeshComponent->CreateMeshSection(Section, Vertices, Triangles, Normals, UVs, TArray<FColor>(),
	                                 TArray<FProcMeshTangent>(), true);
	// Apply the material
	MeshComponent->SetMaterial(Section, Material);

	// Log
	UE_LOG(TerrainActor, Display, TEXT("Mesh section %d created (Vertices: %d, Triangles: %d)"), Section,
	       Vertices.Num(), Triangles.Num());
}

/**
 * Generates the mesh data for the water mesh.
 * 
 * @return Mesh data struct. 
 */
FMeshData ATerrainActor::GenerateWaterMeshData() const
{
	// The mesh data struct
	auto MeshData = FMeshData();

	// Calculate the height of the vertices
	const auto Height = HeightUnit * 3.0 - WaterOffset;

	// Iterate over all tiles
	for (const auto& Tile : Tiles)
	{
		if (Tile.Position.Z <= 0 || HasCoast(Tile))
		{
			for (auto Row = 0; Row < 8; Row++)
			{
				for (auto Col = 0; Col < 9 + Row; Col++)
				{
					const auto Offset = Col * 68 + Row * 64;

					AddVertex(MeshData, Tile, 16 + Offset, Height, true, true);
					AddVertex(MeshData, Tile, 80 + Offset, Height, true, true);
					AddVertex(MeshData, Tile, 148 + Offset, Height, true, true);

					AddVertex(MeshData, Tile, 1996 - Offset, Height, true, true);
					AddVertex(MeshData, Tile, 2128 - Offset, Height, true, true);
					AddVertex(MeshData, Tile, 2064 - Offset, Height, true, true);

					if (Col < 8 + Row)
					{
						AddVertex(MeshData, Tile, 16 + Offset, Height, true, true);
						AddVertex(MeshData, Tile, 148 + Offset, Height, true, true);
						AddVertex(MeshData, Tile, 84 + Offset, Height, true, true);

						AddVertex(MeshData, Tile, 1996 - Offset, Height, true, true);
						AddVertex(MeshData, Tile, 2060 - Offset, Height, true, true);
						AddVertex(MeshData, Tile, 2128 - Offset, Height, true, true);
					}
				}
			}
		}
	}

	// Return the mesh data struct
	return MeshData;
}

/**
 * Generates the mesh data for the terrain mesh.
 * 
 * @return Mesh data struct. 
 */
FMeshData ATerrainActor::GenerateTerrainMeshData() const
{
	// The mesh data struct
	auto MeshData = FMeshData();

	// Iterate over all tiles
	for (const auto& Tile : Tiles)
	{
		// Generate the center part of the tile mesh
		GenerateTerrainTileCenter(MeshData, Tile);
		// Iterate over all directions of the tile
		for (const auto Direction : TEnumRange<ETileDirection>())
		{
			// Get neighbour heights
			const auto Heights = GetNeighbourHeights(Tile, Direction);
			const auto LeftZ = Heights[0];
			const auto CenterZ = Heights[1];
			const auto RightZ = Heights[2];
			// Generate the inner edge
			GenerateTerrainTileInnerEdge(MeshData, Tile, Direction, CenterZ);
			// Generate the outer edge
			GenerateTerrainTileOuterEdge(MeshData, Tile, Direction, LeftZ, CenterZ, RightZ);
			// Generate the inner corners
			GenerateTerrainTileInnerCorners(MeshData, Tile, Direction, LeftZ, CenterZ, RightZ);
			// Generate the outer corners
			GenerateTerrainTileOuterCorners(MeshData, Tile, Direction, LeftZ, CenterZ, RightZ);
			// Generate the center wall
			GenerateTerrainTileCenterWall(MeshData, Tile, Direction, CenterZ);
			// Generate left side wall
			GenerateTerrainTileSideWall(MeshData, Tile, Direction, CenterZ, LeftZ, 27, 11);
			// Generate right side wall
			GenerateTerrainTileSideWall(MeshData, Tile, Direction, CenterZ, RightZ, 17, 33);
			// Generate left side corner wall
			GenerateTerrainTileLeftCornerWall(MeshData, Tile, Direction, CenterZ, LeftZ);
			// Generate right side corner wall
			GenerateTerrainTileRightCornerWall(MeshData, Tile, Direction, CenterZ, RightZ);
		}
	}

	// Return the mesh data struct
	return MeshData;
}

/**
 * Generates the mesh data for the center part of the tile mesh.
 * 
 * @param MeshData The mesh data struct. 
 * @param Tile The tile the mesh data is generated for.
 */
void ATerrainActor::GenerateTerrainTileCenter(FMeshData& MeshData, const FTile& Tile) const
{
	// Rows of the mesh
	for (auto Row = 0; Row < 4; Row++)
	{
		// Columns of the mesh
		for (auto Col = 0; Col < Row + 5; Col++)
		{
			// Calculate offset of vertex position index
			const auto Offset = Col * 68 + Row * 64;
			// Add triangle part 1
			AddVertex(MeshData, Tile, 544 + Offset, 1.0);
			AddVertex(MeshData, Tile, 608 + Offset, 1.0);
			AddVertex(MeshData, Tile, 676 + Offset, 1.0);
			// Add triangle part 2
			AddVertex(MeshData, Tile, 1600 - Offset, 1.0);
			AddVertex(MeshData, Tile, 1536 - Offset, 1.0);
			AddVertex(MeshData, Tile, 1468 - Offset, 1.0);
			if (Col < Row + 4)
			{
				// Add triangle part 3
				AddVertex(MeshData, Tile, 544 + Offset, 1.0);
				AddVertex(MeshData, Tile, 676 + Offset, 1.0);
				AddVertex(MeshData, Tile, 612 + Offset, 1.0);
				// Add triangle part 4
				AddVertex(MeshData, Tile, 1600 - Offset, 1.0);
				AddVertex(MeshData, Tile, 1468 - Offset, 1.0);
				AddVertex(MeshData, Tile, 1532 - Offset, 1.0);
			}
		}
	}
}

/**
 * Generates the mesh data for the inner edge in the specified direction for the specified tile.
 * 
 * @param MeshData The mesh data struct. 
 * @param Tile The tile the mesh data is generated for.
 * @param Direction The direction of the inner edge.
 * @param CenterZ The height of the neighbour in the specified direction.
 */
void ATerrainActor::GenerateTerrainTileInnerEdge(FMeshData& MeshData, const FTile& Tile, const ETileDirection Direction,
                                                 const int32 CenterZ) const
{
	// Calculate additional heights
	const auto H1 = Tile.Position.Z > CenterZ ? 0.5 : Tile.Position.Z < CenterZ ? 1.5 : 1.0;
	const auto H2 = Tile.Position.Z > CenterZ ? 0.0 : Tile.Position.Z < CenterZ ? 2.0 : 1.0;
	// Calculate mesh data
	for (auto Col = 0; Col < 4; Col++)
	{
		AddTriangle(MeshData, Tile, Direction, Col, 1.0, Col + 6, H1, Col + 1, 1.0);
		AddTriangle(MeshData, Tile, Direction, Col, 1.0, Col + 12, H2, Col + 6, H1);
		AddTriangle(MeshData, Tile, Direction, Col + 1, 1.0, Col + 6, H1, Col + 13, H2);
		AddTriangle(MeshData, Tile, Direction, Col + 6, H1, Col + 12, H2, Col + 13, H2);
	}
}

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
void ATerrainActor::GenerateTerrainTileOuterEdge(FMeshData& MeshData, const FTile& Tile, const ETileDirection Direction,
                                                 const int32 LeftZ, const int32 CenterZ, const int32 RightZ) const
{
	const auto TileZ = Tile.Position.Z;

	const auto Hc0 = TileZ > CenterZ ? (CenterZ - TileZ) * 4.0 + 4.0 : TileZ < CenterZ ? 2.0 : 1.0;
	const auto Hc1 = TileZ > CenterZ ? (CenterZ - TileZ) * 4.0 + 3.5 : TileZ < CenterZ ? 2.5 : 1.0;
	const auto Hc2 = TileZ > CenterZ ? (CenterZ - TileZ) * 4.0 + 3.0 : TileZ < CenterZ ? 3.0 : 1.0;

	const auto Hl0 = TileZ != CenterZ ? Hc0 : TileZ > LeftZ ? 0.0 : TileZ < LeftZ ? 2.0 : Hc0;
	const auto Hl1 = TileZ != CenterZ ? Hc1 : TileZ > LeftZ ? 0.5 : TileZ < LeftZ ? 1.5 : Hc1;
	const auto Hl2 = TileZ != CenterZ ? Hc2 : Hl0;

	const auto Hr0 = TileZ != CenterZ ? Hc0 : TileZ > RightZ ? 0.0 : TileZ < RightZ ? 2.0 : Hc0;
	const auto Hr1 = TileZ != CenterZ ? Hc1 : TileZ > RightZ ? 0.5 : TileZ < RightZ ? 1.5 : Hc1;
	const auto Hr2 = TileZ != CenterZ ? Hc2 : Hr0;

	AddTriangle(MeshData, Tile, Direction, 11, Hl0, 19, Hl1, 12, Hc0);
	AddTriangle(MeshData, Tile, Direction, 11, Hl0, 27, Hl2, 19, Hl1);
	AddTriangle(MeshData, Tile, Direction, 12, Hc0, 19, Hl1, 28, Hc2);
	AddTriangle(MeshData, Tile, Direction, 19, Hl1, 27, Hl2, 28, Hc2);

	for (auto Col = 1; Col < 5; Col++)
	{
		AddTriangle(MeshData, Tile, Direction, Col + 11, Hc0, Col + 19, Hc1, Col + 12, Hc0);
		AddTriangle(MeshData, Tile, Direction, Col + 11, Hc0, Col + 27, Hc2, Col + 19, Hc1);
		AddTriangle(MeshData, Tile, Direction, Col + 12, Hc0, Col + 19, Hc1, Col + 28, Hc2);
		AddTriangle(MeshData, Tile, Direction, Col + 19, Hc1, Col + 27, Hc2, Col + 28, Hc2);
	}

	AddTriangle(MeshData, Tile, Direction, 16, Hc0, 24, Hr1, 17, Hr0);
	AddTriangle(MeshData, Tile, Direction, 16, Hc0, 32, Hc2, 24, Hr1);
	AddTriangle(MeshData, Tile, Direction, 17, Hr0, 24, Hr1, 33, Hr2);
	AddTriangle(MeshData, Tile, Direction, 24, Hr1, 32, Hc2, 33, Hr2);
}

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
void ATerrainActor::GenerateTerrainTileInnerCorners(FMeshData& MeshData, const FTile& Tile,
                                                    const ETileDirection Direction, const int32 LeftZ,
                                                    const int32 CenterZ, const int32 RightZ) const
{
	// Get left corner heights
	const auto LeftHeights = CalculateInnerCornerHeights(Tile, CenterZ, LeftZ);
	const auto Lh0 = LeftHeights[0];
	const auto Lh1 = LeftHeights[1];
	const auto Lh2 = LeftHeights[2];
	const auto Lh3 = LeftHeights[3];
	// Generate left corner mesh data
	AddTriangle(MeshData, Tile, Direction, 0, Lh0, 5, Lh2, 12, Lh1);
	AddTriangle(MeshData, Tile, Direction, 5, Lh2, 11, Lh3, 12, Lh1);

	// Get right corner heights
	const auto RightHeights = CalculateInnerCornerHeights(Tile, CenterZ, RightZ);
	const auto Rh0 = RightHeights[0];
	const auto Rh1 = RightHeights[1];
	const auto Rh2 = RightHeights[2];
	const auto Rh3 = RightHeights[3];
	// Generate right corner mesh data
	AddTriangle(MeshData, Tile, Direction, 4, Rh0, 16, Rh1, 10, Rh2);
	AddTriangle(MeshData, Tile, Direction, 10, Rh2, 16, Rh1, 17, Rh3);
}

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
void ATerrainActor::GenerateTerrainTileOuterCorners(FMeshData& MeshData, const FTile& Tile,
                                                    const ETileDirection Direction, const int32 LeftZ,
                                                    const int32 CenterZ, const int32 RightZ) const
{
	// Get left corner heights
	const auto LeftHeights = CalculateOuterCornerHeights(Tile, CenterZ, LeftZ);
	const auto Lh0 = LeftHeights[0];
	const auto Lh1 = LeftHeights[1];
	const auto Lh2 = LeftHeights[2];
	const auto Lh3 = LeftHeights[3];
	// Generate left corner mesh data
	AddTriangle(MeshData, Tile, Direction, 11, Lh0, 18, Lh2, 27, Lh1);
	AddTriangle(MeshData, Tile, Direction, 18, Lh2, 26, Lh3, 27, Lh1);

	// Get right corner heights
	const auto RightHeights = CalculateOuterCornerHeights(Tile, CenterZ, RightZ);
	const auto Rh0 = RightHeights[0];
	const auto Rh1 = RightHeights[1];
	const auto Rh2 = RightHeights[2];
	const auto Rh3 = RightHeights[3];
	// Generate right corner mesh data
	AddTriangle(MeshData, Tile, Direction, 17, Rh0, 33, Rh1, 25, Rh2);
	AddTriangle(MeshData, Tile, Direction, 25, Rh2, 33, Rh1, 34, Rh3);
}

/**
 * Generates the mesh data for the center wall in the specified direction for the specified tile.
 * 
 * @param MeshData The mesh data struct. 
 * @param Tile The tile the mesh data is generated for.
 * @param Direction The direction of the center wall.
 * @param CenterZ The height of the neighbour in the specified direction.
 */
void ATerrainActor::GenerateTerrainTileCenterWall(FMeshData& MeshData, const FTile& Tile,
                                                  const ETileDirection Direction, const int32 CenterZ) const
{
	// Calculate the difference between the tile height and the height of the center neighbour
	const auto Diff = Tile.Position.Z - CenterZ;
	// If the height are greater than one, we need a wall.
	if (Diff > 1)
	{
		// Iterate for every level of the differnce that is greater than one
		for (auto Level = 0; Level < Diff - 1; Level++)
		{
			// Calculate heights
			const auto H0 = Level * -4.0;
			const auto H1 = H0 - WallEdgeHeight;
			const auto H2 = H0 - 2.0;
			const auto H3 = H0 - (4.0 - WallEdgeHeight);
			const auto H4 = H0 - 4.0;
			// Iterate over all columns of the wall.
			for (auto Col = 0; Col < 6; Col++)
			{
				// Calculate local vertex indices
				const auto I0 = Col + 11;
				const auto I1 = Col + 12;
				// Add upper edge
				AddTriangle(MeshData, Tile, Direction, I1, H0, I0, H0, I0, H1);
				AddTriangle(MeshData, Tile, Direction, I1, H0, I0, H1, I1, H1);
				// Add upper wall
				AddTriangle(MeshData, Tile, Direction, I1, H1, I0, H1, I0, H2);
				AddTriangle(MeshData, Tile, Direction, I1, H1, I0, H2, I1, H2);
				// Add lower wall
				AddTriangle(MeshData, Tile, Direction, I1, H2, I0, H2, I0, H3);
				AddTriangle(MeshData, Tile, Direction, I1, H2, I0, H3, I1, H3);
				// Add lower edge
				AddTriangle(MeshData, Tile, Direction, I1, H3, I0, H3, I0, H4);
				AddTriangle(MeshData, Tile, Direction, I1, H3, I0, H4, I1, H4);
			}
		}
	}
}

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
void ATerrainActor::GenerateTerrainTileSideWall(FMeshData& MeshData, const FTile& Tile, const ETileDirection Direction,
                                                const int32 CenterZ, const int32 SideZ, const int32 Index0,
                                                const int32 Index1) const
{
	// Calculate the rows needed
	const auto Rows = FMath::Min(Tile.Position.Z, CenterZ) - SideZ - 1;
	// Iterate over every row
	for (auto Row = 0; Row < Rows; Row++)
	{
		// Calculate vertex heights
		const auto H0 = (Tile.Position.Z > CenterZ ? (CenterZ - Tile.Position.Z) * 4.0 : 0) - Row * 4.0;
		const auto H1 = H0 - WallEdgeHeight;
		const auto H2 = H0 - 2.0;
		const auto H3 = H0 - (4.0 - WallEdgeHeight);
		const auto H4 = H0 - 4.0;

		// Add upper edge
		AddTriangle(MeshData, Tile, Direction, Index0, H0, Index1, H0, Index1, H1);
		AddTriangle(MeshData, Tile, Direction, Index0, H0, Index1, H1, Index0, H1);
		// Add upper wall
		AddTriangle(MeshData, Tile, Direction, Index0, H1, Index1, H1, Index1, H2);
		AddTriangle(MeshData, Tile, Direction, Index0, H1, Index1, H2, Index0, H2);
		// Add lower wall
		AddTriangle(MeshData, Tile, Direction, Index0, H2, Index1, H2, Index1, H3);
		AddTriangle(MeshData, Tile, Direction, Index0, H2, Index1, H3, Index0, H3);
		// Add lower edge
		AddTriangle(MeshData, Tile, Direction, Index0, H3, Index1, H3, Index1, H4);
		AddTriangle(MeshData, Tile, Direction, Index0, H3, Index1, H4, Index0, H4);
	}
}

/**
 * Generates the mesh data for the left side corner wall in the specified direction for the specified tile.
 * 
 * @param MeshData The mesh data struct. 
 * @param Tile The tile the mesh data is generated for.
 * @param Direction The direction of the left side corner wall.
 * @param CenterZ The height of the neighbour in the specified direction.
 * @param LeftZ The height of the neighbour of the left side of the specified direction.
 */
void ATerrainActor::GenerateTerrainTileLeftCornerWall(FMeshData& MeshData, const FTile& Tile,
                                                      const ETileDirection Direction, const int32 CenterZ,
                                                      const int32 LeftZ) const
{
	// Check, if there are a gap between tile and left neightbour that needs to be filled
	if (Tile.Position.Z > CenterZ && Tile.Position.Z - 1 > LeftZ && CenterZ > LeftZ)
	{
		// Calculate heights
		const auto H0 = (Tile.Position.Z - CenterZ - 1) * -4.0;
		const auto H1 = H0 - WallEdgeHeight;
		const auto H2 = H0 - 1.0;
		const auto H3 = H0 - 2.0;
		const auto H4 = H0 - (4.0 - WallEdgeHeight);
		const auto H5 = H0 - 4.0;

		// Add necessary triangles
		AddTriangle(MeshData, Tile, Direction, 11, H0, 11, H1, 27, H2);
		AddTriangle(MeshData, Tile, Direction, 11, H1, 27, H3, 27, H2);
		AddTriangle(MeshData, Tile, Direction, 11, H1, 11, H3, 27, H3);
		AddTriangle(MeshData, Tile, Direction, 11, H3, 27, H4, 27, H3);
		AddTriangle(MeshData, Tile, Direction, 11, H3, 11, H4, 27, H4);
		AddTriangle(MeshData, Tile, Direction, 11, H4, 27, H5, 27, H4);
		AddTriangle(MeshData, Tile, Direction, 11, H4, 11, H5, 27, H5);
	}

	// Check, if there is a little corner to be filled when the one neighbour is lower and the other is higher.
	if (Tile.Position.Z < CenterZ && Tile.Position.Z > LeftZ)
	{
		// Add necessary triangles
		AddTriangle(MeshData, Tile, Direction, 0, 1.0, 5, 0.5, 5, 1.5);
		AddTriangle(MeshData, Tile, Direction, 11, 2.0, 5, 1.5, 11, 0.5);
		AddTriangle(MeshData, Tile, Direction, 11, 0.5, 5, 1.5, 5, 0.5);
		AddTriangle(MeshData, Tile, Direction, 11, 0.5, 5, 0.5, 11, 0.0);
		AddTriangle(MeshData, Tile, Direction, 27, 3.0, 11, 2.0, 27, 2.0);
		AddTriangle(MeshData, Tile, Direction, 27, 2.0, 11, 2.0, 11, 0.5);
		AddTriangle(MeshData, Tile, Direction, 27, 2.0, 11, 0.5, 27, WallEdgeHeight);
		AddTriangle(MeshData, Tile, Direction, 27, WallEdgeHeight, 11, 0.5, 11, 0.0);
		AddTriangle(MeshData, Tile, Direction, 27, WallEdgeHeight, 11, 0.0, 27, 0.0);
	}
}

/**
 * Generates the mesh data for the right side corner wall in the specified direction for the specified tile.
 * 
 * @param MeshData The mesh data struct. 
 * @param Tile The tile the mesh data is generated for.
 * @param Direction The direction of the right side corner wall.
 * @param CenterZ The height of the neighbour in the specified direction.
 * @param RightZ The height of the neighbour of the right side of the specified direction.
 */
void ATerrainActor::GenerateTerrainTileRightCornerWall(FMeshData& MeshData, const FTile& Tile,
                                                       const ETileDirection Direction, const int32 CenterZ,
                                                       const int32 RightZ) const
{
	// Check, if there are a gap between tile and right neightbour that needs to be filled
	if (Tile.Position.Z > CenterZ && Tile.Position.Z - 1 > RightZ && CenterZ > RightZ)
	{
		// Calculate heights
		const auto H0 = (Tile.Position.Z - CenterZ - 1) * -4.0;
		const auto H1 = H0 - WallEdgeHeight;
		const auto H2 = H0 - 1.0;
		const auto H3 = H0 - 2.0;
		const auto H4 = H0 - (4.0 - WallEdgeHeight);
		const auto H5 = H0 - 4.0;

		// Add necessary triangles
		AddTriangle(MeshData, Tile, Direction, 17, H0, 33, H2, 17, H1);
		AddTriangle(MeshData, Tile, Direction, 17, H1, 33, H2, 33, H3);
		AddTriangle(MeshData, Tile, Direction, 17, H1, 33, H3, 17, H3);
		AddTriangle(MeshData, Tile, Direction, 17, H3, 33, H3, 33, H4);
		AddTriangle(MeshData, Tile, Direction, 17, H3, 33, H4, 17, H4);
		AddTriangle(MeshData, Tile, Direction, 17, H4, 33, H4, 33, H5);
		AddTriangle(MeshData, Tile, Direction, 17, H4, 33, H5, 17, H5);
	}

	// Check, if there is a little corner to be filled when the one neighbour is lower and the other is higher.
	if (Tile.Position.Z < CenterZ && Tile.Position.Z > RightZ)
	{
		// Add necessary triangles
		AddTriangle(MeshData, Tile, Direction, 4, 1.0, 10, 1.5, 10, 0.5);
		AddTriangle(MeshData, Tile, Direction, 17, 2.0, 17, 0.5, 10, 1.5);
		AddTriangle(MeshData, Tile, Direction, 10, 1.5, 17, 0.5, 10, 0.5);
		AddTriangle(MeshData, Tile, Direction, 10, 0.5, 17, 0.5, 17, 0.0);
		AddTriangle(MeshData, Tile, Direction, 17, 2.0, 33, 3.0, 33, 2.0);
		AddTriangle(MeshData, Tile, Direction, 17, 2.0, 33, 2.0, 33, WallEdgeHeight);
		AddTriangle(MeshData, Tile, Direction, 17, 2.0, 33, WallEdgeHeight, 17, 0.5);
		AddTriangle(MeshData, Tile, Direction, 17, 0.5, 33, WallEdgeHeight, 33, 0.0);
		AddTriangle(MeshData, Tile, Direction, 17, 0.5, 33, 0.0, 17, 0.0);
	}
}

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
void ATerrainActor::AddVertex(FMeshData& MeshData, const FTile& Tile, const int32 Index, const double Height,
                              const bool Absolute, const bool NoDistortion) const
{
	// Get the coordinates of the position vector of the tile.
	const auto Px = Tile.Position.X * TILE_WIDTH
		+ ((Tile.Position.Y & 1) == 0 ? 0.0 : TILE_WIDTH / 2.0)
		- TILE_WIDTH / 2.0;
	const auto Py = Tile.Position.Y * 0.75 - 0.5;
	const auto Pz = Tile.Position.Z * HeightUnit * 4.0;
	// Vertex grid cooridinates
	const auto Vy = Index / 33;
	const auto Vx = Index - Vy * 33;
	// Create the vertex vector
	const auto Vertex = FVector(
		(Px + TILE_WIDTH / 32.0 * Vx) * Scale,
		(Py + 0.015625 * Vy) * Scale,
		Absolute ? Height * Scale : (Pz + Height * HeightUnit) * Scale
	);
	// Create vertex key
	const auto Key = UE::Geometry::FVector3i(
		FMath::RoundToInt32(Vertex.X * KEY_FACTOR / Scale),
		FMath::RoundToInt32(Vertex.Y * KEY_FACTOR / Scale),
		FMath::RoundToInt32(Vertex.Z * KEY_FACTOR / Scale)
	);
	// If the vertex don't exist, add it to the map and the array
	if (!MeshData.VertexMap.Contains(Key))
	{
		// Calculation distortion
		const auto N = NoDistortion
			               ? FVector::Zero()
			               : Noise(Vertex, NoiseParameterX, NoiseParameterY, NoiseParameterZ);
		// Add to map and array
		MeshData.VertexMap.Add(Key, MeshData.VertexArray.Num());
		MeshData.VertexArray.Add(Vertex + N);
		// Update bounds
		MeshData.MinimalX = FMath::Min(MeshData.MinimalX, Vertex.X);
		MeshData.MaximalX = FMath::Max(MeshData.MaximalX, Vertex.X);
		MeshData.MinimalY = FMath::Min(MeshData.MinimalY, Vertex.Y);
		MeshData.MaximalY = FMath::Max(MeshData.MaximalY, Vertex.Y);
	}
	// Add new triangle index
	MeshData.TriangleArray.Add(MeshData.VertexMap[Key]);
}

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
void ATerrainActor::AddTriangle(FMeshData& MeshData, const FTile& Tile, const ETileDirection Direction,
                                const int32 Index0, const double Height0, const int32 Index1, const double Height1,
                                const int32 Index2, const double Height2) const
{
	// Add the three vertices of the triangle
	AddVertex(MeshData, Tile, EdgeVertices[Direction][Index0], Height0);
	AddVertex(MeshData, Tile, EdgeVertices[Direction][Index1], Height1);
	AddVertex(MeshData, Tile, EdgeVertices[Direction][Index2], Height2);
}

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
TArray<int32> ATerrainActor::GetNeighbourHeights(const FTile& Tile, const ETileDirection Direction) const
{
	// Get direction of left and right neighbour
	const auto LeftDirection = static_cast<ETileDirection>(Direction > TopRight ? Direction - 1 : TopLeft);
	const auto RightDirection = static_cast<ETileDirection>(Direction < TopLeft ? Direction + 1 : TopRight);

	// Create result array
	auto Heights = TArray<int32>();

	// Store left neighbour height
	const auto LeftTile = GetNeighbour(Tile, LeftDirection);
	Heights.Add(LeftTile != nullptr ? LeftTile->Position.Z : Tile.Position.Z);
	// Store center neighbour height
	const auto CenterTile = GetNeighbour(Tile, Direction);
	Heights.Add(CenterTile != nullptr ? CenterTile->Position.Z : Tile.Position.Z);
	// Store right neighbour height
	const auto RightTile = GetNeighbour(Tile, RightDirection);
	Heights.Add(RightTile != nullptr ? RightTile->Position.Z : Tile.Position.Z);

	// Return the result array
	return Heights;
}

/**
 * Returns a pointer to the neighbour tile in the specified direction for the specified tile. If there is no tile
 * in that direction, a <i>nullptr</i> is returned.
 * 
 * @param Tile The tile for which the neighbour is determined.
 * @param Direction The direction of the neighbour tile.
 * 
 * @return A pointer to the neighbour tile or <i>nullptr</i>. 
 */
const FTile* ATerrainActor::GetNeighbour(const FTile& Tile, const ETileDirection Direction) const
{
	// Get coordinates of the tile
	const auto X = Tile.Position.X;
	const auto Y = Tile.Position.Y;

	// Return the neighbour for the specified direction
	switch (Direction)
	{
	case TopRight:
		// Return top right neighbour
		return GetTile((Y & 1) == 0 ? X : X + 1, Y + 1);
	case Right:
		// Return right neighbour
		return GetTile(X + 1, Y);
	case BottomRight:
		// Return bottom right neighbour
		return GetTile((Y & 1) == 0 ? X : X + 1, Y - 1);
	case BottomLeft:
		// Return bottom left neighbour
		return GetTile((Y & 1) == 0 ? X - 1 : X, Y - 1);
	case Left:
		// Return right neighbour
		return GetTile(X - 1, Y);
	case TopLeft:
		// Return bottom left neighbour
		return GetTile((Y & 1) == 0 ? X - 1 : X, Y + 1);
	default:
		// Invalid direction
		return nullptr;
	}
}

/**
 * Returns a pointer to the tile at the specified coordinates. If the coordinates are invalid, a <i>nullptr</i> is
 * returned.
 * 
 * @param X The X coordinate of the tile. 
 * @param Y The Y coordinate of the tile.
 * 
 * @return A pointer to the tile or a <i>nullptr</i>. 
 */
const FTile* ATerrainActor::GetTile(const int32 X, const int32 Y) const
{
	// Check validity of the specified coordinates
	if (X >= 0 && X < SizeX && Y >= 0 && Y < SizeY)
	{
		// Return a pointer to the tile
		return &Tiles[X + Y * SizeX];
	}

	// Invalid coordinates, return null pointer
	return nullptr;
}

/**
 * Checks if there is water in the specified direction for the specified tile.
 * 
 * @param Tile The tile to be checked for having a coast.
 * @param Direction The direction that is checked.
 * 
 * @return If there is water in the specified direction then <b>true</b>, otherwise <b>false</b>. 
 */
bool ATerrainActor::HasCoast(const FTile& Tile, const ETileDirection Direction) const
{
	// Get the neighbour tile
	const auto Neighbour = GetNeighbour(Tile, Direction);
	// Check, if the neighbour is water
	return Neighbour != nullptr ? Neighbour->Position.Z <= 0 : false;
}

/**
 * Checks if there is water in any direction of the specified tile.
 * 
 * @param Tile The tile to be checked for having a coast.
 * 
 * @return If there is water in at least one direction then <b>true</b>, otherwise <b>false</b>. 
 */
bool ATerrainActor::HasCoast(const FTile& Tile) const
{
	// Iterate over all directions
	for (const auto Direction : TEnumRange<ETileDirection>())
	{
		// Check for coast
		if (HasCoast(Tile, Direction))
		{
			// At least one coast was found, thats enough
			return true;
		}
	}
	// No coast found
	return false;
}

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
TArray<double> ATerrainActor::CalculateInnerCornerHeights(const FTile& Tile, const int32 CenterZ, const int32 SideZ)
{
	if (Tile.Position.Z < CenterZ)
	{
		// Tile is lower as the center neighbour
		return {1.0, 2.0, 1.5, 2.0};
	}
	if (Tile.Position.Z == CenterZ && Tile.Position.Z < SideZ)
	{
		// Tile is on the same height as the center neighbour but lower as the side neighbour
		return {1.0, 1.0, 1.5, 2.0};
	}
	if (Tile.Position.Z == CenterZ && Tile.Position.Z > SideZ)
	{
		// Tile is on the same height as the center neighbour and higher as the side neighbour
		return {1.0, 1.0, 0.5, 0.0};
	}
	if (Tile.Position.Z > CenterZ)
	{
		// Tile is higher as the center neighbour
		return {1.0, 0.0, 0.5, 0.0};
	}
	// Return the default array
	return {1.0, 1.0, 1.0, 1.0};
}

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
TArray<double> ATerrainActor::CalculateOuterCornerHeights(const FTile& Tile, const int32 CenterZ, const int32 SideZ)
{
	// Calculate center and side height difference
	const auto CenterDiff = (CenterZ - Tile.Position.Z) * 4.0;
	const auto SideDiff = (SideZ - Tile.Position.Z) * 4.0;

	if (Tile.Position.Z < CenterZ && Tile.Position.Z <= SideZ)
	{
		// Tile is lower than the center neighbour and lower or equal to the side neighbour.
		return {2.0, 3.0, 2.5, 3.0};
	}
	if (Tile.Position.Z == CenterZ && Tile.Position.Z < SideZ)
	{
		// Tile is on the same height as the center neighbour but lower as the side neighbour
		return {2.0, 2.0, 2.5, 3.0};
	}
	if (Tile.Position.Z > SideZ && CenterZ > SideZ)
	{
		// Tile and center neighbour are both higher than the side neighbour
		return {SideDiff + 4.0, SideDiff + 4.0, SideDiff + 3.5, SideDiff + 3.0};
	}
	if (Tile.Position.Z > CenterZ && CenterZ <= SideZ)
	{
		// Tile is higher than the center neighbour and center neighbour is lower or equal to the side neighbour
		return {CenterDiff + 4.0, CenterDiff + 3.0, CenterDiff + 3.5, CenterDiff + 3.0};
	}
	// Return the default array
	return {1.0, 1.0, 1.0, 1.0};
}

/**
 * Calculates the array of the UV coordinates for the specified mesh data.
 * 
 * @param MeshData The mesh data struct.
 * 
 * @return Array of 2D vectors. 
 */
TArray<FVector2D> ATerrainActor::CalculateUVArray(const FMeshData& MeshData)
{
	// Create the UV array
	auto UVs = TArray<FVector2D>();

	// Get width and length of the entire mesh
	const auto DiffX = MeshData.MaximalX - MeshData.MinimalX;
	const auto DiffY = MeshData.MaximalY - MeshData.MinimalY;

	// Iterate over all vertices
	for (const auto Vertex : MeshData.VertexArray)
	{
		// Calculate the UV coordinates and add the vector to the array
		const auto U = Vertex.X / DiffX;
		const auto V = Vertex.Y / DiffY;
		UVs.Add(FVector2D(U, V));
	}

	//Log
	UE_LOG(TerrainActor, Display, TEXT("UV coordinates calculates (%f x %f)"), DiffX, DiffY);

	// Return the array
	return UVs;
}

/**
 * Calculates the array of the normal vectors for the specified mesh data.
 * 
 * @param MeshData The mesh data struct.
 * 
 * @return Array of vectors. 
 */
TArray<FVector> ATerrainActor::CalculateNormalArray(const FMeshData& MeshData)
{
	// Create the array for the normals
	auto Normals = TArray<FVector>();
	// Initialize the array with zero vectors
	for (auto I = 0; I < MeshData.VertexArray.Num(); I++)
	{
		Normals.Add(FVector::Zero());
	}

	// Get the count of triangles
	const auto TriangleCount = MeshData.TriangleArray.Num() / 3;
	// Iterate over every triangle
	for (auto I = 0; I < TriangleCount; I++)
	{
		// Get the indices of the vertices of the triangle
		const auto I0 = MeshData.TriangleArray[I * 3];
		const auto I1 = MeshData.TriangleArray[I * 3 + 1];
		const auto I2 = MeshData.TriangleArray[I * 3 + 2];
		// Get the vertices of the triangle
		const auto V0 = MeshData.VertexArray[I0];
		const auto V1 = MeshData.VertexArray[I1];
		const auto V2 = MeshData.VertexArray[I2];
		// Calculate the normal vector
		const auto NV = FVector::CrossProduct(V1 - V0, V2 - V0) * -1.0;
		// Add the new normal vector to the normal vectors in the array
		Normals[I0] += NV;
		Normals[I1] += NV;
		Normals[I2] += NV;
	}

	// Normalize all normal vectors
	for (auto I = 0; I < Normals.Num(); I++)
	{
		Normals[I].Normalize(1.0);
	}

	// Return the array
	return Normals;
}

/**
 * Calculates a noise value for the specified coordinates and the noise parameter.
 * 
 * @param Px X coordinate. 
 * @param Py Y coordinate.
 * @param Params Noise parameter.
 * 
 * @return The noise value. 
 */
double ATerrainActor::Noise(const double Px, const double Py, const FNoiseParameter& Params)
{
	// Normalize coordinates
	const auto Nx = Px / Params.Size.X + Params.Offset.X;
	const auto Ny = Py / Params.Size.Y + Params.Offset.Y;

	// Cumulative noise value
	auto E = 0.0;
	// Cumulative frequency value
	auto F = 0.0;
	// Iterate over all octaves
	for (auto Oct = 0; Oct < Params.Octaves; Oct++)
	{
		const auto Fv = Params.Frequency * FMath::Pow(2.0, Oct);
		E += FMath::PerlinNoise2D(FVector2D(Nx * Fv, Ny * Fv));
		F += 1.0 / Fv;
	}
	// Normalize result noise value
	E = FMath::Pow(E / F, Params.Redistribution);
	// Apply amplitude and return the noise value
	return E * Params.Amplitude;
}

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
FVector ATerrainActor::Noise(const FVector& Vertex, const FNoiseParameter& ParamsX, const FNoiseParameter& ParamsY,
                             const FNoiseParameter& ParamsZ)
{
	const auto X = Noise(Vertex.Y, Vertex.Z, ParamsX);
	const auto Y = Noise(Vertex.X, Vertex.Z, ParamsY);
	const auto Z = Noise(Vertex.X, Vertex.Y, ParamsZ);
	return FVector(X, Y, Z);
}
