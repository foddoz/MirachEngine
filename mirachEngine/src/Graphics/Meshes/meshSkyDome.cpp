#include "meshSkyDome.hpp"
#include "../Texturing/texture.hpp"
#include "../graphics.hpp"

//#include <glad/gl.h>
#include <iostream>
#include <cmath>

// Constructor initializes member values
MeshSkyDome::MeshSkyDome()
    : 
    m_numRings(0), 
    m_numSectors(0), 
    m_radius(0.0f)
{

}

// Generate CPU vertex/index data for a hemisphere sky dome
void MeshSkyDome::GenerateCPUData(int numRings, int numSectors, float radius, const std::string& textureFile)
{
    m_numRings = numRings;
    m_numSectors = numSectors;
    m_radius = radius;

    m_textureFileName = textureFile; // Store texture path for use in UploadToGPU

    // Allocate one texture slot
    m_textures.resize(1);
    m_textures[0] = new Texture(TextureTarget::Texture2D); // Texture created but not yet loaded
    m_textures[0]->LoadFromFile(m_textureFileName);

    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;

    // Generate vertex positions and UVs for the hemisphere
    for (int r = 0; r <= m_numRings; r++) 
    {
        float theta = (PI / 2.0f) * (float)r / m_numRings;
        float y = sinf(theta) * m_radius;
        float r0 = cosf(theta) * m_radius;

        for (int s = 0; s <= m_numSectors; s++) 
        {
            float phi = TWO_PI * (float)s / m_numSectors;
            float x = r0 * cosf(phi);
            float z = r0 * sinf(phi);

            float u = (float)s / m_numSectors;
            float v = (float)r / m_numRings;

            vertices.push_back({ Vector3f{ x, y, z }, Vector2f{ u, v } });
        }
    }

    // Generate triangle indices
    for (int r = 0; r < m_numRings; r++) 
    {
        for (int s = 0; s < m_numSectors; s++) 
        {
            int curRow = r * (m_numSectors + 1);
            int nextRow = (r + 1) * (m_numSectors + 1);

            indices.push_back(curRow + s);
            indices.push_back(nextRow + s);
            indices.push_back(nextRow + s + 1);

            indices.push_back(curRow + s);
            indices.push_back(nextRow + s + 1);
            indices.push_back(curRow + s + 1);
        }
    }

    m_cpuVertexData = std::move(vertices);
    m_cpuIndexData = std::move(indices);
}

// Upload geometry and texture to GPU
void MeshSkyDome::UploadToGPU()
{
    PopulateBuffers(); // Create VAO/VBO/IBO

    m_indexCount = static_cast<GLsizei>(m_cpuIndexData.size());

    // Load texture from file
    if (m_textures[0])
    {
        m_textures[0]->UploadToGPU();
    }
}

// Creates GPU buffers and attribute pointers
void MeshSkyDome::PopulateBuffers()
{
    using namespace Graphics;

    GenVertexArray(m_vao);
    BindVertexArray(m_vao);

    GenBuffer(m_vbo);
    BindBuffer(BufferTarget::ArrayBuffer, m_vbo);
    BufferData(BufferTarget::ArrayBuffer,
        m_cpuVertexData.size() * sizeof(Vertex),
        m_cpuVertexData.data(),
        BufferUsage::StaticDraw);

    GenBuffer(m_ibo);
    BindBuffer(BufferTarget::ElementArrayBuffer, m_ibo);
    BufferData(BufferTarget::ElementArrayBuffer,
        m_cpuIndexData.size() * sizeof(unsigned int),
        m_cpuIndexData.data(),
        BufferUsage::StaticDraw);

    // === Vertex Attributes ===
    EnableVertexAttrib(POSITION_LOCATION);
    SetVertexAttribPointer(POSITION_LOCATION, 3, DataType::Float, false, sizeof(Vertex), (const void*)offsetof(Vertex, pos));

    EnableVertexAttrib(TEX_COORD_LOCATION);
    SetVertexAttribPointer(TEX_COORD_LOCATION, 2, DataType::Float, false, sizeof(Vertex), (const void*)offsetof(Vertex, tex));

    // === Finalize VAO ===
    UnbindVertexArray();
}

// Cleanup resources (calls Mesh3D base delete)
void MeshSkyDome::Delete()
{
    Mesh3D::Delete();
    // Free CPU-side data
    m_cpuVertexData.clear();
    m_cpuIndexData.clear();
}

// Returns a MeshRenderer for ECS usage
MeshRenderer MeshSkyDome::ToRenderer()
{
    MeshRenderer renderer;

    MeshLOD lod;
    lod.vao = m_vao;
    lod.vbo = m_vbo;
    lod.ibo = m_ibo;
    lod.indexCount = m_indexCount;
    lod.localAABB = m_AABB;

    renderer.lods.push_back(lod);

    renderer.pipeline = m_pipeline;
    renderer.textures = m_textures;
    renderer.minHeight = 0.0f;
    renderer.maxHeight = 0.0f;
    renderer.textureUniformNames.push_back("gSampler");

    return renderer;
}
