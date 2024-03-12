using Terrain;
using UnityEditor;
using UnityEngine;

namespace Editor.Terrain
{
    /// <summary>
    /// Custom editor for the TerrainManager class.
    /// </summary>
    [CustomEditor(typeof(TerrainManager))]
    public class TerrainManagerEditor : UnityEditor.Editor
    {
        /// <summary>
        /// Represents a horizontal plane in three-dimensional space.
        /// </summary>
        private static readonly Plane Plane = new(Vector3.up, 0f);

        /// <summary>
        /// The terrain manager bound to this editor.
        /// </summary>
        private TerrainManager m_TerrainManager;

        /// <summary>
        /// TerrainRenderer class represents a renderer for terrain objects in the game.
        /// </summary>
        private TerrainRenderer m_TerrainRenderer;

        private WaterRenderer m_WaterRenderer;

        public override void OnInspectorGUI()
        {
            base.OnInspectorGUI();

            if (GUILayout.Button("Clear All"))
            {
                m_TerrainManager.InitializeTiles(false);
                m_TerrainRenderer.Render(m_TerrainManager);
                m_WaterRenderer.Render(m_TerrainManager);
            }

            if (GUILayout.Button("Fill All"))
            {
                m_TerrainManager.InitializeTiles(true);
                m_TerrainRenderer.Render(m_TerrainManager);
                m_WaterRenderer.Render(m_TerrainManager);
            }
        }

        /// <summary>
        /// This method is called when the script instance is being loaded or reloaded.
        /// </summary>
        private void OnEnable()
        {
            m_TerrainManager = (TerrainManager)target;
            m_TerrainRenderer = m_TerrainManager.GetComponent<TerrainRenderer>();
            m_WaterRenderer = m_TerrainManager.GetComponent<WaterRenderer>();
        }

        /// <summary>
        /// Executes when the Scene View is being rendered and allows for custom handling of GUI events in the scene.
        /// </summary>
        private void OnSceneGUI()
        {
            // Adds a default, passive-focus control for keyboard navigation in the GUI.
            HandleUtility.AddDefaultControl(GUIUtility.GetControlID(FocusType.Passive));

            // Generates a ray from the camera through a screen point.
            var ray = HandleUtility.GUIPointToWorldRay(Event.current.mousePosition);
            // Checks if the ray hits the default plane. 
            if (Plane.Raycast(ray, out var distance))
            {
                // Get the point where the ray hits the plane
                var point = ray.GetPoint(distance);

                // Get the hexagon position from the world point
                var hexPos = TerrainUtilities.GetHexagonPosition(point);

                // Check if the position is valid
                if (m_TerrainManager.terrainSize.IsValid(hexPos))
                {
                    // Draw the selected hexagon
                    DrawHexagon(hexPos, Color.green, 2f);

                    // Show the current position coordinates in the editor GUI
                    Handles.BeginGUI();
                    GUI.Label
                    (
                        new Rect(10, 10, 200, 30),
                        $"X = {hexPos.X}, Y = {hexPos.Y}, Z = {hexPos.Z}"
                    );
                    Handles.EndGUI();

                    // Check for mouse button event
                    if (Event.current.type == EventType.MouseUp)
                    {
                        var offset = Event.current.shift ? 1 : Event.current.control ? -1 : 0;
                        if (offset != 0)
                        {
                            // Get the tile from the selected position 
                            var tile = m_TerrainManager[hexPos];

                            if (tile != null)
                                // Increase height of the current tile
                                tile.UpdateHeight(offset);
                            else
                                // Create a new tile at the selected position
                                m_TerrainManager[hexPos] = new TerrainTile(m_TerrainManager, hexPos);

                            // Render the terrain and water
                            m_TerrainRenderer.Render(m_TerrainManager);
                            m_WaterRenderer.Render(m_TerrainManager);
                        }
                    }
                }


                // Repaint the scene
                SceneView.RepaintAll();
            }
        }

        /// <summary>
        /// Draws a hexagon at the specified position with the given color and thickness.
        /// </summary>
        /// <param name="position">The position of the hexagon.</param>
        /// <param name="color">The color of the hexagon.</param>
        /// <param name="thickness">The thickness of the lines forming the hexagon.</param>
        private static void DrawHexagon(HexagonPosition position, Color color, float thickness)
        {
            // Get the center point
            var center = position.Center;

            // Set the color
            Handles.color = color;

            // Draw the hexagon
            Handles.DrawAAPolyLine
            (
                thickness,
                new Vector3(center.x, center.y, center.z - 0.5f),
                new Vector3(center.x - TerrainUtilities.HexagonHalfWidth, center.y, center.z - 0.25f),
                new Vector3(center.x - TerrainUtilities.HexagonHalfWidth, center.y, center.z + 0.25f),
                new Vector3(center.x, center.y, center.z + 0.5f),
                new Vector3(center.x + TerrainUtilities.HexagonHalfWidth, center.y, center.z + 0.25f),
                new Vector3(center.x + TerrainUtilities.HexagonHalfWidth, center.y, center.z - 0.25f),
                new Vector3(center.x, center.y, center.z - 0.5f)
            );
        }
    }
}