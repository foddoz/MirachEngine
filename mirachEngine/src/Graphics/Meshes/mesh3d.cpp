#include "mesh3d.hpp"
#include <vector>
#include <iostream>

// Default constructor: initialize OpenGL handles and pipeline
Mesh3D::Mesh3D()
    : 
    m_vao(0), 
    m_vbo(0), 
    m_ibo(0), 
    m_pipeline(0), 
    m_indexCount(0)
{

}

// Copy constructor: copy essential mesh state (except GPU handles)
Mesh3D::Mesh3D(const Mesh3D& other)
    : 
    m_pipeline(other.m_pipeline), 
    m_AABB(other.m_AABB), 
    m_indexCount(other.m_indexCount)
{
    // Do not copy GPU resource handles; they must be generated per-instance
    m_vao = 0;
    m_vbo = 0;
    m_ibo = 0;

    // Shallow copy texture pointers; actual texture ownership should be handled by derived classes
    m_textures = other.m_textures;
}

/**
 * @brief Sets the graphics pipeline (OpenGL shader program).
 * @param pipeline OpenGL program ID to assign.
 */
void Mesh3D::SetPipeline(unsigned int pipeline)
{
    m_pipeline = pipeline;
}

/**
 * @brief Deletes OpenGL buffer resources and releases associated textures.
 *
 * Deletes VBO, IBO, VAO, and any Texture objects created on the heap.
 */
void Mesh3D::Delete()
{
    // Delete vertex/index buffers and VAO from GPU
    Graphics::DeleteBuffer(m_vbo);
    Graphics::DeleteBuffer(m_ibo);
    Graphics::DeleteVertexArray(m_vao);

    // Reset GPU handles to 0
    m_vbo = m_ibo = m_vao = 0;

    // Clean up all textures
    for (auto& texture : m_textures)
    {
        if (texture)
        {
            texture->Delete();  // Release GPU texture resource
            delete texture;     // Delete heap-allocated object
            texture = nullptr;
        }
    }

    // Clear the texture list
    m_textures.clear();
}

/**
 * @brief Retrieves the assigned shader pipeline (OpenGL program ID).
 * @return The current pipeline ID.
 */
unsigned int Mesh3D::getPipeline() const
{
    return m_pipeline;
}
