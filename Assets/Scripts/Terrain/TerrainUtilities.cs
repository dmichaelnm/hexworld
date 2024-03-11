using UnityEngine;

namespace Terrain
{
    /// <summary>
    /// Utility class for handling terrain-related calculations.
    /// </summary>
    public static class TerrainUtilities
    {
        /// <summary>
        /// The width of a hexagon in 3D space.
        /// </summary>
        public static readonly float HexagonWidth = Mathf.Sqrt(3f) / 2f;

        /// <summary>
        /// The half width of a hexagon in 3D space.
        /// </summary>
        public static readonly float HexagonHalfWidth = HexagonWidth / 2f;

        /// <summary>
        /// Represents the height unit of a hexagon.
        /// </summary>
        public static readonly float HexagonHeightUnit = 0.05f;
        
        /// <summary>
        /// Calculates the hexagon position for a given point in 3D space.
        /// </summary>
        /// <param name="point">The point in 3D space.</param>
        /// <returns>The hexagon position as a HexagonPosition object.</returns>
        public static HexagonPosition GetHexagonPosition(Vector3 point)
        {
            var q = (Mathf.Sqrt(3f) / 3f * point.x - 1f / 3f * point.z) / 0.5f;
            var r = 2f / 3f * point.z / 0.5f;
            var s = -q - r;

            var roundedQ = Mathf.Round(q);
            var roundedR = Mathf.Round(r);
            var roundedS = Mathf.Round(s);

            var diffQ = Mathf.Abs(roundedQ - q);
            var diffR = Mathf.Abs(roundedR - r);
            var diffS = Mathf.Abs(roundedS - s);

            if (diffQ > diffR && diffQ > diffS)
                q = -roundedR - roundedS;
            else if (diffR > diffS)
                r = -roundedQ - roundedS;

            var x = Mathf.RoundToInt(q);
            var y = Mathf.RoundToInt(point.y / (4 * HexagonHeightUnit));
            var z = Mathf.RoundToInt(r);

            x += (z - (z & 1)) / 2;
            
            return new HexagonPosition(x, y, z);
        }
    }
}