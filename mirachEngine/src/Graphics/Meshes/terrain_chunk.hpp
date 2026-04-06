#ifndef TERRAINCHUNK_HPP
#define TERRAINCHUNK_HPP

#include "meshTerrain.hpp"

/**
 * @class TerrainChunk
 * @brief Represents a sub-section of a larger terrain for LOD or streaming.
 *
 * Inherits from MeshTerrain and extracts its data from a global heightmap.
 * This allows terrain to be split into smaller pieces for performance and culling.
 */
class TerrainChunk : public MeshTerrain
{
public:
    /**
     * @brief Default constructor. Initializes internal state.
     */
    TerrainChunk();

    /**
     * @brief Generates mesh data for this chunk from a global heightmap source.
     *
     * This function extracts a rectangular region of height values from a larger terrain
     * and constructs vertex/index buffers only for that portion.
     *
     * @param globalHeightmap The full terrain heightmap shared across all chunks.
     * @param startX The X-coordinate (column) in the global heightmap to start this chunk.
     * @param startZ The Z-coordinate (row) in the global heightmap to start this chunk.
     * @param chunkSize The number of vertices in width and depth this chunk should span.
     * @param globalWidth The full width of the global heightmap (used for indexing).
     * @param minHeight Minimum expected height value (used for AABB normalization).
     * @param maxHeight Maximum expected height value.
     * @param textureScale UV scaling factor for this chunk's texture coordinates.
     * @param textureFileNames A list of texture file paths to use on this chunk.
     */
    void GenerateCPUData(const std::vector<float>& globalHeightmap,
        int startX, int startZ,
        int chunkSize,
        int globalWidth,
        float minHeight, float maxHeight,
        float textureScale,
        const std::vector<std::string>& textureFileNames);

    /**
     * @brief Uploads this chunk’s vertex/index/texture data to the GPU.
     */
    void UploadToGPU() override;

    /**
     * @brief Returns the world-space origin of this chunk in terrain units.
     *
     * Typically used for placing the chunk correctly in the world or computing transforms.
     * @return Vector3f representing bottom-left corner of the chunk.
     */
    Vector3f GetOrigin() const;

    /**
     * @brief Shares preloaded textures across multiple chunks to reduce memory usage.
     *
     * Replaces this chunk's texture list with a reference to shared ones.
     * Textures are not copied or loaded again.
     *
     * @param textures List of preloaded texture pointers.
     */
    void SetSharedTextures(const std::vector<Texture*>& textures);

private:
    int m_startX; ///< Start X index in global heightmap
    int m_startZ; ///< Start Z index in global heightmap
};

#endif // TERRAINCHUNK_HPP
