using System;
using UnityEngine;

namespace Terrain
{
    [Serializable]
    public struct TerrainDistortion
    {
        public Vector2 xOffset;
        public Vector2 zOffset;
        public float horizontalFrequency;
        public float horizontalAmplitude;
        public float verticalFrequency;
        public float verticalAmplitude;
    }
}