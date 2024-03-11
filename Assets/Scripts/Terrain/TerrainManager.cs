using UnityEngine;

namespace Terrain
{
    public class TerrainManager : MonoBehaviour
    {
        /// <summary>
        /// Represents the size of the terrain counted in hex tiles.
        /// </summary>
        public TerrainSize terrainSize;

        /// <summary>
        /// Gets the TerrainTile at the specified index.
        /// </summary>
        /// <param name="index">The index of the TerrainTile to get.</param>
        /// <returns>The TerrainTile at the specified index.</returns>
        public TerrainTile this[int index]
        {
            // Getter
            get
            {
                if (m_TerrainTiles.Length != terrainSize.size)
                    m_TerrainTiles = new TerrainTile[terrainSize.size];

                return m_TerrainTiles[index];
            }
        }

        /// <summary>
        /// Retrieves or sets the TerrainTile at the specified position in the terrain.
        /// </summary>
        /// <param name="position">The position of the TerrainTile to be retrieved or set.</param>
        /// <returns>The TerrainTile at the specified position.</returns>
        public TerrainTile this[HexagonPosition position]
        {
            // Getter
            get
            {
                if (m_TerrainTiles.Length != terrainSize.size)
                    m_TerrainTiles = new TerrainTile[terrainSize.size];

                return terrainSize.IsValid(position) ? m_TerrainTiles[terrainSize.IndexOf(position)] : null;
            }

            // Setter
            set
            {
                if (m_TerrainTiles.Length != terrainSize.size)
                    m_TerrainTiles = new TerrainTile[terrainSize.size];

                m_TerrainTiles[terrainSize.IndexOf(position)] = value;
            }
        }

        /// <summary>
        /// Array of the terrain tiles.
        /// </summary>
        private TerrainTile[] m_TerrainTiles;

        public void InitializeTiles(bool fill)
        {
            m_TerrainTiles = new TerrainTile[terrainSize.size];
            if (fill)
            {
                for (var i = 0; i < m_TerrainTiles.Length; i++)
                    m_TerrainTiles[i] = new TerrainTile(this, terrainSize.PositionOf(i));
            }
        }
    }
}