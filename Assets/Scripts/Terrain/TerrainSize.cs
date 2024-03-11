using System;

namespace Terrain
{
    /// <summary>
    /// Represents the size of a terrain in terms of hex tiles.
    /// </summary>
    [Serializable]
    public struct TerrainSize
    {
        /// <summary>
        /// Represents the width of the terrain counted in hex tiles.
        /// </summary>
        public int width;

        /// <summary>
        /// Represents the length of the terrain counted in hex tiles.
        /// </summary>
        public int length;

        /// <summary>
        /// Returns the count of tiles that are needed for this terrain size.
        /// </summary>
        public int size => width * length;
        
        /// <summary>
        /// Finds the index of the given HexagonPosition in a collection.
        /// </summary>
        /// <param name="position">The HexagonPosition to find the index of.</param>
        /// <returns>
        /// The index of the HexagonPosition in a collection.
        /// </returns>
        public int IndexOf(HexagonPosition position)
        {
            return position.X + position.Z * width;
        }

        /// <summary>
        /// Calculates the position of a hexagon in a hexagonal grid system given its index and height.
        /// </summary>
        /// <param name="index">The index of the hexagon.</param>
        /// <param name="height">The height of the hexagon (default is 0).</param>
        /// <returns>The HexagonPosition representing the position of the hexagon.</returns>
        public HexagonPosition PositionOf(int index, int height = 0)
        {
            var z = index / width;
            var x = index - z * width;
            return new HexagonPosition(x, height, z);
        }
        
        /// <summary>
        /// Determines if the given hexagon position is valid within the specified boundaries. </summary>
        /// <param name="hexagonPosition">The hexagon position to check.</param>
        /// <returns>
        /// <c>true</c> if the hexagon position is within the boundaries; otherwise, <c>false</c>.
        /// </returns>
        public bool IsValid(HexagonPosition hexagonPosition)
        {
            return hexagonPosition.X >= 0 && hexagonPosition.X < width &&
                   hexagonPosition.Z >= 0 && hexagonPosition.Z < length;
        }
    }
}
