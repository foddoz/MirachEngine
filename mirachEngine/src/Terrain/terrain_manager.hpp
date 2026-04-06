#ifndef TERRAIN_MANAGER_HPP
#define TERRAIN_MANAGER_HPP

#include "../Graphics/Meshes/terrain_chunk.hpp"

#include "../ECS/entity_manager.hpp"
#include "../Transform/transform.hpp"
#include "../Graphics/Rendering/mesh_renderer.hpp"
#include "../Physics/physics_system.hpp"

#include <vector>
#include <string>
#include <memory>
#include <mutex>

/**
 * @class TerrainManager
 * @brief Handles generation, GPU upload, and ECS/physics registration of terrain chunks.
 *
 * This class is responsible for dividing a large heightmap into chunks, generating
 * mesh data for each chunk, uploading it to the GPU, and registering each chunk
 * with the ECS and physics systems for rendering and collision.
 */
class TerrainManager 
{
public:
    /**
     * @brief Initializes terrain system settings and global parameters.
     *
     * @param heightmap Reference to the full terrain heightmap (row-major).
     * @param terrainSize Size (width/height) of the full terrain grid.
     * @param chunkSize Size of each terrain chunk (square).
     * @param minHeight Minimum possible height value.
     * @param maxHeight Maximum possible height value.
     * @param textureScale Texture tiling scale factor.
     * @param textureFiles List of texture file paths for terrain layers.
     */
    void Initialize(const std::vector<float>& heightmap,
        int terrainSize,
        int chunkSize,
        float minHeight,
        float maxHeight,
        float textureScale,
        const std::vector<std::string>& textureFiles);

    /**
     * @brief Generates all terrain chunks from the global heightmap.
     *
     * Must be called after Initialize().
     */
    void GenerateAllChunks();

    /**
     * @brief Uploads all generated chunks to the GPU (VAOs, VBOs, textures).
     */
    void UploadChunksToGPU();

    /**
     * @brief Registers all chunks as ECS entities with rendering and physics components.
     *
     * @param entityManager Reference to the ECS EntityManager.
     * @param physicsSystem Reference to the PhysicsSystem.
     * @param terrainShader Shader program ID used for terrain rendering.
     */
    void RegisterChunksToECS(EntityManager& entityManager,
        PhysicsSystem& physicsSystem,
        unsigned int terrainShader);

    /**
     * @brief Registers a single monolithic terrain mesh into the ECS.
     *
     * Used when not chunking terrain.
     *
     * @param entityManager Reference to the ECS EntityManager.
     * @param physicsSystem Reference to the PhysicsSystem.
     * @param terrainShader Shader program ID used for rendering.
     * @param terrain Reference to the MeshTerrain object.
     */
    void RegisterToECS(EntityManager& entityManager,
        PhysicsSystem& physicsSystem,
        unsigned int terrainShader,
        MeshTerrain& terrain);

    /**
     * @brief Returns all terrain chunks managed by this class.
     * @return Const reference to a vector of unique pointers to TerrainChunk.
     */
    const std::vector<std::unique_ptr<TerrainChunk>>& GetChunks() const;

private:
    /// Pointer to the global terrain heightmap (not owned).
    const std::vector<float>* m_globalHeightmap = nullptr;

    /// Size of the full terrain grid (number of vertices along one axis).
    int m_terrainSize = 0;

    /// Size of each terrain chunk.
    int m_chunkSize = 0;

    /// Minimum terrain elevation.
    float m_minHeight = 0.0f;

    /// Maximum terrain elevation.
    float m_maxHeight = 0.0f;

    /// Scale factor for texture UV tiling.
    float m_textureScale = 1.0f;

    /// Filenames of the textures used for terrain blending.
    std::vector<std::string> m_textureFileNames;

    /// List of all generated terrain chunks.
    std::vector<std::unique_ptr<TerrainChunk>> m_chunks;

    /// Shared texture pointers used by all terrain chunks.
    std::vector<Texture*> m_sharedTextures;

    /// Mutex for thread-safe chunk generation (if multithreaded).
    std::mutex m_chunkMutex;
};

#endif // TERRAIN_MANAGER_HPP
