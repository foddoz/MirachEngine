#ifndef MESH3D_HPP
#define MESH3D_HPP

#include "../../Utilities/utils.hpp"
#include "../Rendering/mesh_renderer.hpp"
#include "../../Math/math.hpp"

#include <string>
#include <vector>

#include "../Texturing/texture.hpp"
#include "../graphics.hpp"

/**
 * @class Mesh3D
 * @brief Abstract base class for rendering and managing 3D mesh data.
 *
 * `Mesh3D` defines a common interface for all 3D meshes in the engine.
 * It encapsulates OpenGL vertex/index buffers, bounding volume, shader pipeline,
 * and associated texture data. Derived classes must implement data upload and rendering conversion.
 */
class Mesh3D 
{
public:
    /**
     * @brief Default constructor initializes OpenGL buffer handles and transforms.
     */
    Mesh3D();

    /**
     * @brief Copy constructor for Mesh3D.
     * @param other The mesh to copy from.
     */
    Mesh3D(const Mesh3D& other);

    /**
     * @brief Uploads the mesh data to the GPU.
     *
     * This function must be overridden by subclasses to populate
     * OpenGL buffers with vertex/index/UV data.
     */
    virtual void UploadToGPU() = 0;

    /**
     * @brief Converts the mesh into a `MeshRenderer` component used by the ECS.
     * @return A fully initialized `MeshRenderer` instance containing rendering info.
     */
    virtual MeshRenderer ToRenderer() = 0;

    /**
     * @brief Assigns a graphics pipeline (shader program) to this mesh.
     * @param pipeline OpenGL shader program ID to bind when rendering.
     */
    void SetPipeline(unsigned int pipeline);

    /**
     * @brief Releases GPU buffers and associated textures.
     *
     * Can be overridden to add additional resource cleanup logic.
     */
    virtual void Delete();

    /**
     * @brief Retrieves the currently assigned graphics pipeline.
     * @return OpenGL shader program ID.
     */
    unsigned int getPipeline() const;

protected:
    unsigned int m_vao;        ///< OpenGL Vertex Array Object handle.
    unsigned int m_indexCount; ///< Number of indices used for drawing (glDrawElements or glDrawArrays).

    unsigned int m_vbo;        ///< Vertex Buffer Object handle.
    unsigned int m_ibo;        ///< Index Buffer Object handle.

    std::vector<Texture*> m_textures; ///< Collection of textures associated with this mesh.

    /**
     * @brief Populates OpenGL buffers for vertex/index/texture data.
     *
     * This must be implemented by subclasses based on the structure of the mesh data.
     */
    virtual void PopulateBuffers() = 0;

    unsigned int m_pipeline;   ///< OpenGL shader program ID assigned to this mesh.

    AABB m_AABB;               ///< Axis-aligned bounding box in local (object) space.

private:
    // Currently no private data members
};

#endif // MESH3D_HPP
