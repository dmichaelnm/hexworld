using UnityEngine;

namespace Terrain
{
    public class TerrainRenderer : MonoBehaviour
    {
        private const float WallEdgeWidth = 0.5f;

        private static readonly int[][] EdgeIndicies =
        {
            // Top Right
            new[]
            {
                1600, 1536, 1472, 1408, 1344,
                1732, 1668, 1604, 1540, 1476, 1412,
                1864, 1800, 1736, 1672, 1608, 1544, 1480,
                1996, 1932, 1868, 1804, 1740, 1676, 1612, 1548,
                2128, 2064, 2000, 1936, 1872, 1808, 1744, 1680, 1616
            },
            // Right
            new[]
            {
                1344, 1212, 1080, 948, 816,
                1412, 1280, 1148, 1016, 884, 752,
                1480, 1348, 1216, 1084, 952, 820, 688,
                1548, 1416, 1284, 1152, 1020, 888, 756, 624,
                1616, 1484, 1352, 1220, 1088, 956, 824, 692, 560
            },
            // Bottom Right
            new[]
            {
                816, 748, 680, 612, 544,
                752, 684, 616, 548, 480, 412,
                688, 620, 552, 484, 416, 348, 280,
                624, 556, 488, 420, 352, 284, 216, 148,
                560, 492, 424, 356, 288, 220, 152, 84, 16
            },
            // Bottom Left
            new[]
            {
                544, 608, 672, 736, 800,
                412, 476, 540, 604, 668, 732,
                280, 344, 408, 472, 536, 600, 664,
                148, 212, 276, 340, 404, 468, 532, 596,
                16, 80, 144, 208, 272, 336, 400, 464, 528
            },
            // Left
            new[]
            {
                800, 932, 1064, 1196, 1328,
                732, 864, 996, 1128, 1260, 1392,
                664, 796, 928, 1060, 1192, 1324, 1456,
                596, 728, 860, 992, 1124, 1256, 1388, 1520,
                528, 660, 792, 924, 1056, 1188, 1320, 1452, 1584
            },
            // Top Left
            new[]
            {
                1328, 1396, 1464, 1532, 1600,
                1392, 1460, 1528, 1596, 1664, 1732,
                1456, 1524, 1592, 1660, 1728, 1796, 1864,
                1520, 1588, 1656, 1724, 1792, 1860, 1928, 1996,
                1584, 1652, 1720, 1788, 1856, 1924, 1992, 2060, 2128
            }
        };

        public Material material;
        public TerrainDistortion distortion;

        public void Render(TerrainManager terrainManager)
        {
            // Remove all child objects
            for (var i = transform.childCount - 1; i >= 0; i--)
            {
                if (Application.isPlaying)
                    Destroy(transform.GetChild(i).gameObject);
                else
                    DestroyImmediate(transform.GetChild(i).gameObject);
            }

            // Create mesh data
            var meshData = new MeshData(distortion);

            // Iterate over all tiles
            for (var i = 0; i < terrainManager.terrainSize.size; i++)
            {
                var tile = terrainManager[i];
                if (tile != null)
                {
                    // Render the center plane.
                    RenderCenter(tile, meshData);
                    // Render edges
                    for (var direction = 0; direction < EdgeIndicies.Length; direction++)
                    {
                        // Get heights of the neighbours
                        var neighbours = tile.GetNeighbours((HexagonDirection)direction);
                        var leftY = neighbours[0] != null
                            ? neighbours[0].position.Y
                            : tile.position.Y;
                        var centerY = neighbours[1] != null
                            ? neighbours[1].position.Y
                            : tile.position.Y;
                        var rightY = neighbours[2] != null
                            ? neighbours[2].position.Y
                            : tile.position.Y;

                        // Render inner edge
                        RenderInnerEdge(tile, direction, centerY, meshData);
                        // Render inner left corner
                        RenderInnerLeftCorner(tile, direction, leftY, centerY, meshData);
                        // Render inner right corner
                        RenderInnerRightCorner(tile, direction, centerY, rightY, meshData);
                        // Render outer edge
                        RenderOuterEdge(tile, direction, leftY, centerY, rightY, meshData);
                        // Render outer left corner
                        RenderOuterLeftCorner(tile, direction, leftY, centerY, meshData);
                        // Render outer right corner
                        RenderOuterRightCorner(tile, direction, centerY, rightY, meshData);
                        // Render center wall
                        RenderCenterWall(tile, direction, centerY, meshData);
                        // Render left wall
                        RenderSideWall(meshData, tile, direction, centerY, leftY, 27, 11);
                        // Render left wall corner
                        RenderLeftWallCorner(meshData, tile, direction, centerY, leftY);
                        // Render right wall
                        RenderSideWall(meshData, tile, direction, centerY, rightY, 17, 33);
                        // Render right wall corner
                        RenderRightWallCorner(meshData, tile, direction, centerY, rightY);
                    }
                }
            }

            // Generate the mesh
            meshData.Apply(gameObject, material);
        }

        private static void RenderCenter(TerrainTile tile, MeshData meshData)
        {
            for (var row = 0; row < 4; row++)
            {
                for (var col = 0; col < 4 + row + 1; col++)
                {
                    var lowerOffset = col * 68 + row * 64;
                    meshData.Add(tile.position, 544 + lowerOffset, 1f);
                    meshData.Add(tile.position, 608 + lowerOffset, 1f);
                    meshData.Add(tile.position, 676 + lowerOffset, 1f);
                    meshData.Add(tile.position, 1600 - lowerOffset, 1f);
                    meshData.Add(tile.position, 1536 - lowerOffset, 1f);
                    meshData.Add(tile.position, 1468 - lowerOffset, 1f);
                    if (col < 4 + row)
                    {
                        meshData.Add(tile.position, 544 + lowerOffset, 1f);
                        meshData.Add(tile.position, 676 + lowerOffset, 1f);
                        meshData.Add(tile.position, 612 + lowerOffset, 1f);
                        meshData.Add(tile.position, 1600 - lowerOffset, 1f);
                        meshData.Add(tile.position, 1468 - lowerOffset, 1f);
                        meshData.Add(tile.position, 1532 - lowerOffset, 1f);
                    }
                }
            }
        }

        private static void RenderInnerEdge(
            TerrainTile tile, int direction, int centerY, MeshData meshData)
        {
            var tileY = tile.position.Y;

            var hc1 = tileY > centerY ? 0.5f : tileY < centerY ? 1.5f : 1f;
            var hc2 = tileY > centerY ? 0.0f : tileY < centerY ? 2.0f : 1f;

            for (var col = 0; col < 4; col++)
            {
                AddTriangle(meshData, tile, direction, col, 1f, col + 6, hc1, col + 1, 1f);
                AddTriangle(meshData, tile, direction, col, 1f, col + 12, hc2, col + 6, hc1);
                AddTriangle(meshData, tile, direction, col + 1, 1f, col + 6, hc1, col + 13, hc2);
                AddTriangle(meshData, tile, direction, col + 6, hc1, col + 12, hc2, col + 13, hc2);
            }
        }

        private static void RenderInnerLeftCorner(
            TerrainTile tile, int direction,
            int leftY, int centerY,
            MeshData meshData)
        {
            var h = CalculateInnerCornerHeights(tile, centerY, leftY);

            AddTriangle(meshData, tile, direction, 0, h[0], 5, h[2], 12, h[1]);
            AddTriangle(meshData, tile, direction, 5, h[2], 11, h[3], 12, h[1]);
        }

        private static void RenderInnerRightCorner(
            TerrainTile tile, int direction,
            int centerY, int rightY,
            MeshData meshData)
        {
            var h = CalculateInnerCornerHeights(tile, centerY, rightY);

            AddTriangle(meshData, tile, direction, 4, h[0], 16, h[1], 10, h[2]);
            AddTriangle(meshData, tile, direction, 10, h[2], 16, h[1], 17, h[3]);
        }

        private static float[] CalculateInnerCornerHeights(TerrainTile tile, int centerY, int sideY)
        {
            var tileY = tile.position.Y;

            if (tileY < centerY)
                return new[] { 1f, 2f, 1.5f, 2f };
            if (tileY == centerY && tileY < sideY)
                return new[] { 1f, 1f, 1.5f, 2f };

            if (tileY > centerY)
                return new[] { 1f, 0f, 0.5f, 0f };
            if (tileY == centerY && tileY > sideY)
                return new[] { 1f, 1f, 0.5f, 0f };

            return new[] { 1f, 1f, 1f, 1f };
        }

        private static void RenderOuterEdge(
            TerrainTile tile, int direction,
            int leftY, int centerY, int rightY,
            MeshData meshData)
        {
            var tileY = tile.position.Y;

            var hc0 = tileY > centerY ? (centerY - tileY) * 4f + 4.0f : tileY < centerY ? 2.0f : 1f;
            var hc1 = tileY > centerY ? (centerY - tileY) * 4f + 3.5f : tileY < centerY ? 2.5f : 1f;
            var hc2 = tileY > centerY ? (centerY - tileY) * 4f + 3.0f : tileY < centerY ? 3.0f : 1f;

            var hl0 = hc0;
            var hl1 = hc1;
            var hl2 = hc2;
            var hr0 = hc0;
            var hr1 = hc1;
            var hr2 = hc2;

            if (tileY == centerY)
            {
                hl0 = tileY > leftY ? 0f : tileY < leftY ? 2f : hc0;
                hl1 = tileY > leftY ? 0.5f : tileY < leftY ? 1.5f : hc1;
                hl2 = hl0;
                hr0 = tileY > rightY ? 0f : tileY < rightY ? 2f : hc0;
                hr1 = tileY > rightY ? 0.5f : tileY < rightY ? 1.5f : hc1;
                hr2 = hr0;
            }

            AddTriangle(meshData, tile, direction, 11, hl0, 19, hl1, 12, hc0);
            AddTriangle(meshData, tile, direction, 11, hl0, 27, hl2, 19, hl1);
            AddTriangle(meshData, tile, direction, 12, hc0, 19, hl1, 28, hc2);
            AddTriangle(meshData, tile, direction, 19, hl1, 27, hl2, 28, hc2);

            for (var col = 1; col < 5; col++)
            {
                AddTriangle(meshData, tile, direction, col + 11, hc0, col + 19, hc1, col + 12, hc0);
                AddTriangle(meshData, tile, direction, col + 11, hc0, col + 27, hc2, col + 19, hc1);
                AddTriangle(meshData, tile, direction, col + 12, hc0, col + 19, hc1, col + 28, hc2);
                AddTriangle(meshData, tile, direction, col + 19, hc1, col + 27, hc2, col + 28, hc2);
            }

            AddTriangle(meshData, tile, direction, 16, hc0, 24, hr1, 17, hr0);
            AddTriangle(meshData, tile, direction, 16, hc0, 32, hc2, 24, hr1);
            AddTriangle(meshData, tile, direction, 17, hr0, 24, hr1, 33, hr2);
            AddTriangle(meshData, tile, direction, 24, hr1, 32, hc2, 33, hr2);
        }

        private static void RenderSideWall(MeshData meshData, TerrainTile tile, int direction, int centerY, int sideY,
            int i0, int i1)
        {
            var tileY = tile.position.Y;

            var rows = Mathf.Min(tileY, centerY) - sideY - 1;
            for (var row = 0; row < rows; row++)
            {
                var h0 = (tileY > centerY ? (centerY - tileY) * 4f : 0f) - row * 4f;
                var h1 = h0 - WallEdgeWidth;
                var h2 = h0 - 2f;
                var h3 = h0 - (4f - WallEdgeWidth);
                var h4 = h0 - 4f;

                AddTriangle(meshData, tile, direction, i0, h0, i1, h0, i1, h1);
                AddTriangle(meshData, tile, direction, i0, h0, i1, h1, i0, h1);
                AddTriangle(meshData, tile, direction, i0, h1, i1, h1, i1, h2);
                AddTriangle(meshData, tile, direction, i0, h1, i1, h2, i0, h2);
                AddTriangle(meshData, tile, direction, i0, h2, i1, h2, i1, h3);
                AddTriangle(meshData, tile, direction, i0, h2, i1, h3, i0, h3);
                AddTriangle(meshData, tile, direction, i0, h3, i1, h3, i1, h4);
                AddTriangle(meshData, tile, direction, i0, h3, i1, h4, i0, h4);
            }
        }

        private static void RenderLeftWallCorner(MeshData meshData, TerrainTile tile, int direction, int centerY,
            int leftY)
        {
            var tileY = tile.position.Y;

            if (tileY > centerY && tileY - 1 > leftY && centerY > leftY)
            {
                var h0 = (tileY - centerY - 1) * -4f;
                var h1 = h0 - WallEdgeWidth;
                var h2 = h0 - 1f;
                var h3 = h0 - 2f;
                var h4 = h0 - (4f - WallEdgeWidth);
                var h5 = h0 - 4f;

                AddTriangle(meshData, tile, direction, 11, h0, 11, h1, 27, h2);
                AddTriangle(meshData, tile, direction, 11, h1, 27, h3, 27, h2);
                AddTriangle(meshData, tile, direction, 11, h1, 11, h3, 27, h3);
                AddTriangle(meshData, tile, direction, 11, h3, 27, h4, 27, h3);
                AddTriangle(meshData, tile, direction, 11, h3, 11, h4, 27, h4);
                AddTriangle(meshData, tile, direction, 11, h4, 27, h5, 27, h4);
                AddTriangle(meshData, tile, direction, 11, h4, 11, h5, 27, h5);
            }

            if (tileY < centerY && tileY > leftY)
            {
                AddTriangle(meshData, tile, direction, 0, 1f, 5, 0.5f, 5, 1.5f);
                AddTriangle(meshData, tile, direction, 11, 2f, 5, 1.5f, 11, 0.5f);
                AddTriangle(meshData, tile, direction, 11, 0.5f, 5, 1.5f, 5, 0.5f);
                AddTriangle(meshData, tile, direction, 11, 0.5f, 5, 0.5f, 11, 0f);
                AddTriangle(meshData, tile, direction, 27, 3f, 11, 2f, 27, 2f);
                AddTriangle(meshData, tile, direction, 27, 2f, 11, 2f, 11, 0.5f);
                AddTriangle(meshData, tile, direction, 27, 2f, 11, 0.5f, 27, WallEdgeWidth);
                AddTriangle(meshData, tile, direction, 27, WallEdgeWidth, 11, 0.5f, 11, 0f);
                AddTriangle(meshData, tile, direction, 27, WallEdgeWidth, 11, 0f, 27, 0f);
            }
        }

        private static void RenderRightWallCorner(MeshData meshData, TerrainTile tile, int direction, int centerY,
            int rightY)
        {
            var tileY = tile.position.Y;
            if (tileY > centerY && tileY - 1 > rightY && centerY > rightY)
            {
                var h0 = (tileY - centerY - 1) * -4f;
                var h1 = h0 - WallEdgeWidth;
                var h2 = h0 - 1f;
                var h3 = h0 - 2f;
                var h4 = h0 - (4f - WallEdgeWidth);
                var h5 = h0 - 4f;

                AddTriangle(meshData, tile, direction, 17, h0, 33, h2, 17, h1);
                AddTriangle(meshData, tile, direction, 17, h1, 33, h2, 33, h3);
                AddTriangle(meshData, tile, direction, 17, h1, 33, h3, 17, h3);
                AddTriangle(meshData, tile, direction, 17, h3, 33, h3, 33, h4);
                AddTriangle(meshData, tile, direction, 17, h3, 33, h4, 17, h4);
                AddTriangle(meshData, tile, direction, 17, h4, 33, h4, 33, h5);
                AddTriangle(meshData, tile, direction, 17, h4, 33, h5, 17, h5);
            }


            if (tileY < centerY && tileY > rightY)
            {
                AddTriangle(meshData, tile, direction, 4, 1f, 10, 1.5f, 10, 0.5f);
                AddTriangle(meshData, tile, direction, 17, 2f, 17, 0.5f, 10, 1.5f);
                AddTriangle(meshData, tile, direction, 10, 1.5f, 17, 0.5f, 10, 0.5f);
                AddTriangle(meshData, tile, direction, 10, 0.5f, 17, 0.5f, 17, 0f);
                AddTriangle(meshData, tile, direction, 17, 2f, 33, 3f, 33, 2f);
                AddTriangle(meshData, tile, direction, 17, 2f, 33, 2f, 33, WallEdgeWidth);
                AddTriangle(meshData, tile, direction, 17, 2f, 33, WallEdgeWidth, 17, 0.5f);
                AddTriangle(meshData, tile, direction, 17, 0.5f, 33, WallEdgeWidth, 33, 0f);
                AddTriangle(meshData, tile, direction, 17, 0.5f, 33, 0f, 17, 0f);
            }
        }

        private static void RenderOuterLeftCorner(
            TerrainTile tile, int direction,
            int leftY, int centerY,
            MeshData meshData)
        {
            var h = CalculateOuterCornerHeights(tile, centerY, leftY);

            AddTriangle(meshData, tile, direction, 11, h[0], 18, h[2], 27, h[1]);
            AddTriangle(meshData, tile, direction, 18, h[2], 26, h[3], 27, h[1]);
        }

        private static void RenderOuterRightCorner(
            TerrainTile tile, int direction,
            int centerY, int rightY,
            MeshData meshData)
        {
            var h = CalculateOuterCornerHeights(tile, centerY, rightY);

            AddTriangle(meshData, tile, direction, 17, h[0], 33, h[1], 25, h[2]);
            AddTriangle(meshData, tile, direction, 25, h[2], 33, h[1], 34, h[3]);
        }

        private static float[] CalculateOuterCornerHeights(TerrainTile tile, int centerY, int sideY)
        {
            var tileY = tile.position.Y;
            var centerDiff = (centerY - tileY) * 4f;
            var sideDiff = (sideY - tileY) * 4;

            if (tileY < centerY && tileY <= sideY)
                return new[] { 2f, 3f, 2.5f, 3f };
            if (tileY == centerY && tileY < sideY)
                return new[] { 2f, 2f, 2.5f, 3f };
            if (tileY != centerY && tileY > sideY && centerY > sideY)
                return new[] { sideDiff + 4f, sideDiff + 4f, sideDiff + 3.5f, sideDiff + 3f };
            if (tileY == centerY && tileY > sideY && centerY > sideY)
                return new[] { sideDiff + 4f, sideDiff + 4f, sideDiff + 3.5f, sideDiff + 3f };
            if (tileY > centerY && centerY <= sideY)
                return new[] { centerDiff + 4f, centerDiff + 3f, centerDiff + 3.5f, centerDiff + 3f };

            return new[] { 1f, 1f, 1f, 1f };
        }

        private static void RenderCenterWall(TerrainTile tile, int direction, int centerY, MeshData meshData)
        {
            var tileY = tile.position.Y;
            var diff = tileY - centerY;
            if (diff > 1)
            {
                for (var level = 0; level < diff - 1; level++)
                {
                    var h0 = level * -4f;
                    var h1 = h0 - WallEdgeWidth;
                    var h2 = h0 - 2f;
                    var h3 = h0 - (4f - WallEdgeWidth);
                    var h4 = h0 - 4f;

                    for (var col = 0; col < 6; col++)
                    {
                        var i0 = col + 11;
                        var i1 = col + 12;

                        AddTriangle(meshData, tile, direction, i1, h0, i0, h0, i0, h1);
                        AddTriangle(meshData, tile, direction, i1, h0, i0, h1, i1, h1);
                        AddTriangle(meshData, tile, direction, i1, h1, i0, h1, i0, h2);
                        AddTriangle(meshData, tile, direction, i1, h1, i0, h2, i1, h2);
                        AddTriangle(meshData, tile, direction, i1, h2, i0, h2, i0, h3);
                        AddTriangle(meshData, tile, direction, i1, h2, i0, h3, i1, h3);
                        AddTriangle(meshData, tile, direction, i1, h3, i0, h3, i0, h4);
                        AddTriangle(meshData, tile, direction, i1, h3, i0, h4, i1, h4);
                    }
                }
            }
        }

        private static void AddTriangle
        (
            MeshData meshData, TerrainTile tile, int direction,
            int index0, float height0,
            int index1, float height1,
            int index2, float height2
        )
        {
            meshData.Add(tile.position, EdgeIndicies[direction][index0], height0);
            meshData.Add(tile.position, EdgeIndicies[direction][index1], height1);
            meshData.Add(tile.position, EdgeIndicies[direction][index2], height2);
        }
    }
}