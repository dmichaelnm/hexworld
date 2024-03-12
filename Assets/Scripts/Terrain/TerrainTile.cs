using System;

namespace Terrain
{
    /// <summary>
    /// Represents a terrain tile.
    /// </summary>
    [Serializable]
    public class TerrainTile
    {
        /// <summary>
        /// Gets the position of the hexagon.
        /// </summary>
        public HexagonPosition position { get; private set; }

        public bool IsWater => position.Y < 0;

        /// <summary>
        /// Represents the terrain manager.
        /// </summary>
        private readonly TerrainManager m_TerrainManager;

        /// <summary>
        /// Represents a terrain tile.
        /// </summary>
        /// <param name="terrainManager">The terrain manager that controls this tile.</param>
        /// <param name="position">The position of this tile.</param>
        public TerrainTile(TerrainManager terrainManager, HexagonPosition position)
        {
            m_TerrainManager = terrainManager;
            this.position = position;
        }

        /// <summary>
        /// Returns the neighbouring terrain tile in the specified direction.
        /// </summary>
        /// <param name="direction">The direction in which to find the neighbour.</param>
        /// <returns>The neighbouring terrain tile in the specified direction.</returns>
        public TerrainTile GetNeighbour(HexagonDirection direction)
        {
            return m_TerrainManager[position + direction];
        }

        /// <summary>
        /// Gets the neighbouring terrain tiles in the specified direction.
        /// </summary>
        /// <param name="direction">The direction to get the neighbours in.</param>
        /// <returns>An array of neighbouring terrain tiles.</returns>
        public TerrainTile[] GetNeighbours(HexagonDirection direction)
        {
            var neighbours = new TerrainTile[3];
            var d = (int)direction;

            neighbours[0] = GetNeighbour((HexagonDirection)(d > 0 ? d - 1 : 5));
            neighbours[1] = GetNeighbour(direction);
            neighbours[2] = GetNeighbour((HexagonDirection)(d < 5 ? d + 1 : 0));

            return neighbours;
        }

        public bool HasCoast()
        {
            for (var d = 0; d < 6; d++)
            {
                if (HasCoast((HexagonDirection)d))
                    return true;
            }

            return false;
        }

        public bool HasCoast(HexagonDirection direction)
        {
            var neighbour = GetNeighbour(direction);
            if (position.Y >= 0)
                return neighbour is { position: { Y: < 0 } };

            return neighbour is { position: { Y: >= 0 } };
        }

        /// <summary>
        /// Updates the height of this terrain tile.
        /// </summary>
        /// <param name="heightOffset">The amount by which to offset the height.</param>
        /// <remarks>
        /// The <paramref name="heightOffset"/> should be positive to increase the height,
        /// and negative to decrease the height.
        /// </remarks>
        public void UpdateHeight(int heightOffset)
        {
            position += heightOffset;
        }
    }
}