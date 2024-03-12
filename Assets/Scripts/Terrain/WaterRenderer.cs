using UnityEngine;

namespace Terrain
{
    public class WaterRenderer : MonoBehaviour
    {
        public Material material;
        public float height;

        public void Render(TerrainManager terrainManager)
        {
            // Remove all child objects
            for (var i = transform.childCount - 1; i >= 0; i--)
            {
                var child = transform.GetChild(i).gameObject;
                if (child.name.StartsWith("Water"))
                {
                    if (Application.isPlaying)
                        Destroy(transform.GetChild(i).gameObject);
                    else
                        DestroyImmediate(transform.GetChild(i).gameObject);
                }
            }

            // Create mesh data
            var meshData = new MeshData(default);
            
            // Iterate over all tiles
            for (var i = 0; i < terrainManager.terrainSize.size; i++)
            {
                var tile = terrainManager[i];
                if (tile != null)
                {
                    if (tile.IsWater || tile.HasCoast())
                    {
                        meshData.Add(tile.position, 16, height, false);
                        meshData.Add(tile.position, 528, height, false);
                        meshData.Add(tile.position, 560, height, false);
                        
                        meshData.Add(tile.position, 528, height, false);
                        meshData.Add(tile.position, 2128, height, false);
                        meshData.Add(tile.position, 560, height, false);
                        
                        meshData.Add(tile.position, 528, height, false);
                        meshData.Add(tile.position, 1584, height, false);
                        meshData.Add(tile.position, 2128, height, false);
                        
                        meshData.Add(tile.position, 2128, height, false);
                        meshData.Add(tile.position, 1616, height, false);
                        meshData.Add(tile.position, 560, height, false);
                    }
                }
            }
            
            // Generate the mesh
            meshData.Apply(gameObject, material, "Water");
        }
    }
}