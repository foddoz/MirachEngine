#include "terrain_chunk.hpp"

TerrainChunk::TerrainChunk()
    : MeshTerrain(), m_startX(0), m_startZ(0)
{
    // Initializes a terrain chunk with default starting coordinates (0,0)
}

/**
 * @brief Generate terrain chunk mesh from a portion of a global heightmap.
 */
void TerrainChunk::GenerateCPUData(const std::vector<float>& globalHeightmap,
    int startX, int startZ,
    int chunkSize,
    int globalWidth,
    float minHeight, float maxHeight,
    float textureScale,
    const std::vector<std::string>& textureFileNames)
{
    // Store chunk's position in the global heightmap
    m_startX = startX;
    m_startZ = startZ;

    const int actualSize = chunkSize + 1; // Add 1 to avoid seams at edges

    // Store terrain properties
    m_terrainSize = actualSize;
    m_width = actualSize;
    m_depth = actualSize;

    m_minHeight = static_cast<int>(minHeight);
    m_maxHeight = static_cast<int>(maxHeight);
    m_textureScale = textureScale;
    m_textureFileNames = textureFileNames;

    // World space dimensions are kept the same as original chunk size
    m_worldSizeX = static_cast<float>(chunkSize);
    m_worldSizeZ = static_cast<float>(chunkSize);

    // Prepare heightmap storage
    m_heightMap.clear();
    m_heightMap.resize(actualSize * actualSize, 0.0f);

    // === Copy height values from global heightmap into this chunk ===
    for (int z = 0; z < actualSize; ++z) 
    {
        for (int x = 0; x < actualSize; ++x) 
        {
            // Clamp to avoid out-of-bounds access on edges
            int globalX = std::min(startX + x, globalWidth - 1);
            int globalZ = std::min(startZ + z, globalWidth - 1); // assumes square

            float height = globalHeightmap[globalZ * globalWidth + globalX];
            m_heightMap[z * actualSize + x] = height;
        }
    }

    // Generate the mesh (vertices, indices, normals)
    CreateTriangleList(actualSize, actualSize);
}

/**
 * @brief Upload terrain chunk texture data to GPU.
 */
void TerrainChunk::UploadToGPU()
{
    // Only upload textures; terrain chunk geometry will be LOD-managed elsewhere
    for (auto& tex : m_textures) 
    {
        if (tex) 
        {
            tex->UploadToGPU();
        }
    }
}

/**
 * @brief Get the world origin of this chunk based on grid location.
 * @return Vector3f representing the chunk's base position in world space.
 */
Vector3f TerrainChunk::GetOrigin() const
{
    // The chunk's world position is derived from its offset in the full terrain
    return Vector3f(static_cast<float>(m_startX), 0.0f, static_cast<float>(m_startZ));
}

/**
 * @brief Replace current textures with shared texture pointers.
 * @param textures Shared terrain texture list (already loaded and uploaded).
 */
void TerrainChunk::SetSharedTextures(const std::vector<Texture*>& textures)
{
    // Avoid reloading the same texture for every chunk
    m_textures = textures;
}
