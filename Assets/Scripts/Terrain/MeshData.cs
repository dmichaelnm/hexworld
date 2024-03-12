using System;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.Rendering;

namespace Terrain
{
    public class MeshData
    {
        /// <summary>
        /// Represents a vertex in a three-dimensional space.
        /// </summary>
        private readonly struct Vertex
        {
            /// <summary>
            /// Normalizing factor.
            /// </summary>
            private const int F = 1000000;

            /// <summary>
            /// Represents the position of the vertex in three dimensional space.
            /// </summary>
            internal readonly Vector3 PositionDistorted;

            internal readonly Vector3 PositionOriginal;

            /// <summary>
            /// Normalized X coordinate.
            /// </summary>
            private readonly int m_X;

            /// <summary>
            /// Normalized Y coordinate.
            /// </summary>
            private readonly int m_Y;

            /// <summary>
            /// Normalized Z coordinate.
            /// </summary>
            private readonly int m_Z;

            /// <summary>
            /// Represents a vertex in a hexagon grid.
            /// </summary>
            /// <param name="position">The position of the hexagon.</param>
            /// <param name="vertexIndex">The index of the vertex.</param>
            /// <param name="heightUnit">The local height unit of the hexagon.</param>
            /// <param name="distortion">Terrain distortion parameter.</param>
            public Vertex(TerrainDistortion distortion, HexagonPosition position, int vertexIndex, float heightUnit,
                bool local = true)
            {
                var center = position.Center;
                var baseX = center.x - TerrainUtilities.HexagonHalfWidth;
                var baseZ = center.z - 0.5f;

                var vz = vertexIndex / 33;
                var vx = vertexIndex - vz * 33;

                var x = baseX + TerrainUtilities.HexagonWidth / 32f * vx;
                var y = local ? center.y + heightUnit * TerrainUtilities.HexagonHeightUnit : heightUnit;
                var z = baseZ + 1f / 64f * vz;

                PositionOriginal = new Vector3(x, y, z);

                var xx = (Mathf.PerlinNoise(
                    (x + distortion.xOffset.x) * distortion.horizontalFrequency,
                    (z + distortion.xOffset.y) * distortion.horizontalFrequency
                ) - 0.5f) * distortion.horizontalAmplitude;
                var xy = (Mathf.PerlinNoise(
                    (x + distortion.xOffset.x) * distortion.horizontalFrequency,
                    (y + distortion.xOffset.y) * distortion.horizontalFrequency
                ) - 0.5f) * distortion.horizontalAmplitude;
                var xz = (Mathf.PerlinNoise(
                    (z + distortion.xOffset.x) * distortion.horizontalFrequency,
                    (y + distortion.xOffset.y) * distortion.horizontalFrequency
                ) - 0.5f) * distortion.horizontalAmplitude;
                x += (xx + xy + xz) / 3f;

                var zx = (Mathf.PerlinNoise(
                    (x + distortion.zOffset.x) * distortion.horizontalFrequency,
                    (z + distortion.zOffset.y) * distortion.horizontalFrequency
                ) - 0.5f) * distortion.horizontalAmplitude;
                var zy = (Mathf.PerlinNoise(
                    (x + distortion.zOffset.x) * distortion.horizontalFrequency,
                    (y + distortion.zOffset.y) * distortion.horizontalFrequency
                ) - 0.5f) * distortion.horizontalAmplitude;
                var zz = (Mathf.PerlinNoise(
                    (z + distortion.zOffset.x) * distortion.horizontalFrequency,
                    (y + distortion.zOffset.y) * distortion.horizontalFrequency
                ) - 0.5f) * distortion.horizontalAmplitude;
                z += (zx + zy + zz) / 3f;

                y += Mathf.PerlinNoise(
                    x * distortion.verticalFrequency,
                    z * distortion.verticalFrequency
                ) * distortion.verticalAmplitude;

                m_X = Mathf.RoundToInt(x * F);
                m_Y = Mathf.RoundToInt(y * F);
                m_Z = Mathf.RoundToInt(z * F);

                PositionDistorted = new Vector3(x, y, z);
            }

            /// <summary>
            /// Determines whether the specified object is equal to the current vertex.
            /// </summary>
            /// <param name="obj">The object to compare with the current vertex.</param>
            /// <returns>true if the specified object is equal to the current vertex; otherwise, false.</returns>
            public override bool Equals(object obj)
            {
                if (obj is Vertex v)
                    return m_X == v.m_X && m_Y == v.m_Y && m_Z == v.m_Z;

                return false;
            }

            /// <summary>
            /// Returns the hash code for this instance.
            /// </summary>
            /// <returns>
            /// A 32-bit signed integer hash code.
            /// </returns>
            public override int GetHashCode()
            {
                return HashCode.Combine(m_X, m_Y, m_Z);
            }
        }

        /// <summary>
        /// Contains the vertices of the mesh and their associates triangle indices.
        /// </summary>
        private readonly Dictionary<Vertex, int> m_Vertices = new();

        /// <summary>
        /// Represents the list of triangle indices for the mesh.
        /// </summary>
        private readonly List<int> m_Triangles = new();

        /// <summary>
        /// The minimum value of the X coordinate of the mesh.
        /// </summary>
        private float m_MinX = float.MaxValue;

        /// <summary>
        /// The maximum value of the X coordinate of the mesh.
        /// </summary>
        private float m_MaxX = float.MinValue;

        /// <summary>
        /// The minimum value of the Z coordinate of the mesh.
        /// </summary>
        private float m_MinZ = float.MaxValue;

        /// <summary>
        /// The maximum value of the Z coordinate of the mesh.
        /// </summary>
        private float m_MaxZ = float.MinValue;

        /// <summary>
        /// Gets the width of the entire mesh.
        /// </summary>
        private float width => m_MaxX - m_MinX;

        /// <summary>
        /// Gets the length of the entire mesh.
        /// </summary>
        private float length => m_MaxZ - m_MinZ;

        /// <summary>
        /// Stores the distortion parameters for terrain mesh generation.
        /// </summary>
        private readonly TerrainDistortion m_Distortion;

        /// <summary>
        /// Creates a new instance of meshdata.
        /// </summary>
        /// <param name="distortion">Distortion parameters</param>
        public MeshData(TerrainDistortion distortion)
        {
            m_Distortion = distortion;
        }

        /// <summary>
        /// Adds a vertex to the hexagon mesh.
        /// </summary>
        /// <param name="position">The position of the hexagon.</param>
        /// <param name="vertexIndex">The index of the vertex within the hexagon.</param>
        /// <param name="heightUnit">The local height unit of the vertex.</param>
        public void Add(HexagonPosition position, int vertexIndex, float heightUnit, bool local = true)
        {
            // Create the vertex instance
            var vertex = new Vertex(m_Distortion, position, vertexIndex, heightUnit, local);

            // Add the vertex only if it does not exist yet in the dictionary.
            if (!m_Vertices.ContainsKey(vertex))
                m_Vertices.Add(vertex, m_Vertices.Count);

            // Add the triangle index for the vertex to the triangles list.
            m_Triangles.Add(m_Vertices[vertex]);

            // Update the boundary values
            m_MinX = Mathf.Min(m_MinX, vertex.PositionOriginal.x);
            m_MaxX = Mathf.Max(m_MaxX, vertex.PositionOriginal.x);
            m_MinZ = Mathf.Min(m_MinZ, vertex.PositionOriginal.z);
            m_MaxZ = Mathf.Max(m_MaxZ, vertex.PositionOriginal.z);
        }

        /// <summary>
        /// Creates a new child object with the mesh generated from this instance.
        /// </summary>
        /// <param name="parent">The parent object to apply the mesh.</param>
        /// <param name="material">The material used for the mesh.</param>
        public void Apply(GameObject parent, Material material, string prefix)
        {
            var vertices = new Vector3[m_Vertices.Count];
            var uv = new Vector2[m_Vertices.Count];

            var index = 0;
            foreach (var vertex in m_Vertices.Keys)
            {
                vertices[index] = vertex.PositionDistorted;
                uv[index] = new Vector2(vertex.PositionOriginal.x / width, vertex.PositionOriginal.z / length);
                index++;
            }

            // ReSharper disable once UseObjectOrCollectionInitializer
            var mesh = new Mesh();
            mesh.name = $"{material.name} Mesh";
            mesh.indexFormat = IndexFormat.UInt32;
            mesh.vertices = vertices;
            mesh.triangles = m_Triangles.ToArray();
            mesh.uv = uv;
            mesh.RecalculateBounds();
            mesh.RecalculateNormals();
            mesh.RecalculateTangents();

            var child = new GameObject($"{prefix}_{material.name}");
            child.transform.SetParent(parent.transform);

            var filter = child.AddComponent<MeshFilter>();
            filter.mesh = mesh;

            var renderer = child.AddComponent<MeshRenderer>();
            renderer.material = material;

            var collider = child.AddComponent<MeshCollider>();
            collider.sharedMesh = mesh;
        }
    }
}