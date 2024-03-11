using System;
using UnityEngine;

namespace Terrain
{
    /// <summary>
    /// Represents the position of a hexagon in a hexagonal grid system.
    /// </summary>
    [Serializable]
    public readonly struct HexagonPosition
    {
        /// <summary>
        /// Determines if two HexagonPositions are equal.
        /// </summary>
        /// <param name="a">The first HexagonPosition to compare.</param>
        /// <param name="b">The second HexagonPosition to compare.</param>
        /// <returns>true if the HexagonPositions are equal, false otherwise.</returns>
        public static bool operator ==(HexagonPosition a, HexagonPosition b)
        {
            return a.X == b.X && a.Y == b.Y && a.Z == b.Z;
        }

        /// <summary>
        /// Determines if two HexagonPositions are not equal.
        /// </summary>
        /// <param name="a">The first HexagonPosition to compare.</param>
        /// <param name="b">The second HexagonPosition to compare.</param>
        /// <returns>true if the HexagonPositions are not equal, false otherwise.</returns>
        public static bool operator !=(HexagonPosition a, HexagonPosition b)
        {
            return a.X != b.X || a.Y != b.Y || a.Z != b.Z;
        }

        /// <summary>
        /// This method overloads the '+' operator for adding an integer height offset to a HexagonPosition object.
        /// </summary>
        /// <param name="a">The original HexagonPosition object.</param>
        /// <param name="heightOffset">The height offset to be added.</param>
        /// <returns>A new HexagonPosition object with the height offset added.</returns>
        public static HexagonPosition operator +(HexagonPosition a, int heightOffset)
        {
            return new HexagonPosition(a.X, a.Y + heightOffset, a.Z);
        }

        /// <summary>
        /// Returns a neighbour position of the specified hexagon position depending on the specified direction.
        /// </summary>
        /// <param name="p">The HexagonPosition to add the HexagonDirection to.</param>
        /// <param name="d">The HexagonDirection to add to the HexagonPosition.</param>
        /// <returns>A new HexagonPosition resulting from adding the HexagonDirection to the HexagonPosition.</returns>
        /// <exception cref="ApplicationException">Thrown when the given HexagonDirection is invalid.</exception>
        public static HexagonPosition operator +(HexagonPosition p, HexagonDirection d)
        {
            return d switch
            {
                HexagonDirection.TopRight => new HexagonPosition((p.Z & 1) == 0 ? p.X : p.X + 1, p.Y, p.Z + 1),
                HexagonDirection.Right => new HexagonPosition(p.X + 1, p.Y, p.Z),
                HexagonDirection.BottomRight => new HexagonPosition((p.Z & 1) == 0 ? p.X : p.X + 1, p.Y, p.Z - 1),
                HexagonDirection.BottomLeft => new HexagonPosition((p.Z & 1) == 0 ? p.X - 1 : p.X, p.Y, p.Z - 1),
                HexagonDirection.Left => new HexagonPosition(p.X - 1, p.Y, p.Z),
                HexagonDirection.TopLeft => new HexagonPosition((p.Z & 1) == 0 ? p.X - 1 : p.X, p.Y, p.Z + 1),
                _ => throw new ApplicationException($"Invalid hexagon direction: {d}")
            };
        }

        /// <summary>
        /// Represents the X coordinate of the position of a hexagon.
        /// </summary>
        public readonly int X;

        /// <summary>
        /// Represents the Y coordinate of the position of a hexagon.
        /// </summary>
        public readonly int Y;

        /// <summary>
        /// Represents the Z coordinate of the position of a hexagon.
        /// </summary>
        public readonly int Z;

        /// <summary>
        /// The center point of the hexagon in the 3D space.
        /// </summary>
        public readonly Vector3 Center;

        /// <summary>
        /// Construct a new HexagonPosition object with the given x, y, and z coordinates.
        /// </summary>
        /// <param name="x">The x coordinate of the hexagon position.</param>
        /// <param name="y">The y coordinate of the hexagon position.</param>
        /// <param name="z">The z coordinate of the hexagon position.</param>
        public HexagonPosition(int x, int y, int z)
        {
            X = x;
            Y = y;
            Z = z;

            // Calculate the center point of the hexagon
            Center = new Vector3
            (
                (z & 1) == 0
                    ? x * TerrainUtilities.HexagonWidth
                    : x * TerrainUtilities.HexagonWidth + TerrainUtilities.HexagonHalfWidth,
                y * 4f * TerrainUtilities.HexagonHeightUnit,
                z * 0.75f
            );
        }

        /// <summary>
        /// Determines whether the current object is equal to the specified object.
        /// </summary>
        /// <param name="obj">The object to compare with the current object.</param>
        /// <returns>true if the current object is equal to the specified object; otherwise, false.</returns>
        public override bool Equals(object obj)
        {
            if (obj is HexagonPosition hp)
                return this == hp;

            return false;
        }

        /// <summary>
        /// Returns the hash code for this object.
        /// </summary>
        /// <returns>
        /// An integer representing the hash code value of the object.
        /// </returns>
        public override int GetHashCode()
        {
            return HashCode.Combine(X, Y, Z);
        }

        /// <summary>
        /// Returns a string that represents the current instance of the HexagonPosition class.
        /// </summary>
        /// <returns>
        /// A string representation of the current HexagonPosition object in the format:
        /// "HexagonPosition[ x = {X}; y = {Y}; z = {Z} ]"
        /// </returns>
        public override string ToString()
        {
            return $"HexagonPosition[ x = {X}; y = {Y}; z = {Z} ]";
        }
    }
}