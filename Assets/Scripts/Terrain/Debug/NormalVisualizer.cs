using System;
using UnityEngine;

namespace Terrain.Debug
{
    public class NormalVisualizer : MonoBehaviour
    {
        private void OnDrawGizmos()
        {
            var meshFilter = GetComponent<MeshFilter>();
            if (meshFilter != null)
            {
                var mesh = meshFilter.sharedMesh;
                var vertices = mesh.vertices;
                var normals = mesh.normals;

                Gizmos.color = Color.cyan;
                for (var i = 0; i < vertices.Length; i++)
                {
                    Gizmos.DrawLine(vertices[i], vertices[i] + normals[i].normalized * 0.02f);
                }
            }
        }
    }
}