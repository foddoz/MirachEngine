// === terrain_manager.cpp ===
#include "terrain_manager.hpp"
#include <iostream>
#include <thread>
#include <mutex>
#include <filesystem>

void TerrainManager::Initialize(const std::vector<float>& heightmap,
    int terrainSize,
    int chunkSize,
    float minHeight,
    float maxHeight,
    float textureScale,
    const std::vector<std::string>& textureFiles)
{
    // Store references and parameters
    m_globalHeightmap = &heightmap;
    m_terrainSize = terrainSize;
    m_chunkSize = chunkSize;
    m_minHeight = minHeight;
    m_maxHeight = maxHeight;
    m_textureScale = textureScale;
    m_textureFileNames = textureFiles;

    // Load shared terrain textures
    for (const auto& path : m_textureFileNames) 
    {
        Texture* tex = new Texture(TextureTarget::Texture2D);
        tex->LoadFromFile(path);                // Load texture from file path
        m_sharedTextures.push_back(tex);        // Store for shared use across chunks
    }
}


void TerrainManager::GenerateAllChunks()
{
    // Ensure thread safety in case of parallel access
    std::lock_guard<std::mutex> lock(m_chunkMutex);

    // Reserve enough space based on expected chunk count
    m_chunks.reserve((m_terrainSize / m_chunkSize) * (m_terrainSize / m_chunkSize));

    // Loop over terrain grid by chunk size
    for (int z = 0; z < m_terrainSize; z += m_chunkSize) 
    {
        for (int x = 0; x < m_terrainSize; x += m_chunkSize) 
        {

            // Create a new chunk
            auto chunk = std::make_unique<TerrainChunk>();

            // Generate mesh data for chunk based on global heightmap
            chunk->GenerateCPUData(*m_globalHeightmap, x, z, m_chunkSize, m_terrainSize,
                m_minHeight, m_maxHeight, m_textureScale, m_textureFileNames);

            // Share texture references
            chunk->SetSharedTextures(m_sharedTextures);

            // Store in chunk list
            m_chunks.push_back(std::move(chunk));
        }
    }
}

void TerrainManager::UploadChunksToGPU()
{
    // Upload all terrain chunks to GPU
    for (auto& chunk : m_chunks) 
    {
        chunk->UploadToGPU(); // Upload VAO/VBO/IBO and bind textures
    }
}

void TerrainManager::RegisterChunksToECS(EntityManager& entityManager, PhysicsSystem& physicsSystem, unsigned int terrainShader)
{
    for (auto& chunk : m_chunks)
    {
        // Set shader pipeline
        chunk->SetPipeline(terrainShader);

        // Convert chunk to renderable ECS component
        MeshRenderer renderer = chunk->ToRenderer();

        // Create a new ECS entity for the chunk
        Entity chunkEntity = entityManager.CreateEntity();

        // Add transform (typically identity for static terrain)
        Transform transform;
        transform.SetPosition(0.0f, 0.0f, 0.0f);
        entityManager.AddComponent<Transform>(chunkEntity, transform);

        // Add render component to ECS
        entityManager.AddComponent<MeshRenderer>(chunkEntity, renderer);

        // Create a static rigidbody for physics representation
        Physics::RigidBody* body = physicsSystem.GetWorld()->CreateRigidBody(transform.GetPosition(), transform.GetQuaternionRotation(), BodyType::KINEMATIC);
        entityManager.AddComponent<Physics::RigidBody*>(chunkEntity, body);

    }
}

const std::vector<std::unique_ptr<TerrainChunk>>& TerrainManager::GetChunks() const
{
    // Return read-only access to all generated chunks
    return m_chunks;
}
