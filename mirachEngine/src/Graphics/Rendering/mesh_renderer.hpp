#ifndef MESH_RENDERER_HPP
#define MESH_RENDERER_HPP

#include <vector>

#include "../Texturing/texture.hpp"
#include "../../Math/math.hpp"
#include "../Meshes/mesh_entry.hpp"

/**
 * @struct MeshLOD
 * @brief Represents a single Level of Detail (LOD) variant of a mesh.
 *
 * Each LOD stores its own VAO, VBO, and IBO, along with index count and a local AABB.
 * Used for LOD switching in rendering systems to improve performance based on distance.
 */
struct MeshLOD
{
    unsigned int vao;        ///< Vertex Array Object ID.
    unsigned int vbo;        ///< Vertex Buffer Object ID.
    unsigned int ibo;        ///< Index Buffer Object ID.
    unsigned int indexCount; ///< Number of indices to render.
    AABB localAABB;          ///< Local-space axis-aligned bounding box.
};

/**
 * @struct MeshRenderer
 * @brief Holds all GPU and rendering information needed for rendering a mesh.
 *
 * This struct is created by Mesh3D-derived classes and used by the RenderSystem
 * to issue draw calls with the appropriate LOD, shader pipeline, and textures.
 */
struct MeshRenderer
{
    std::vector<MeshLOD> lods; ///< Mesh data for each level of detail.

    unsigned int pipeline = 0; ///< OpenGL shader program or graphics pipeline ID.

    std::vector<Texture*> textures; ///< Pointers to texture objects used by this mesh.
    std::vector<std::string> textureUniformNames; ///< Uniform names for each texture in the shader.

    float minHeight = 0.0f; ///< Minimum height value (used for terrain height-based blending).
    float maxHeight = 0.0f; ///< Maximum height value.

    std::vector<MeshEntry> subMeshes; ///< Submesh support for complex multi-material models.

    int currentLOD = 0; ///< Currently active LOD index for rendering.
};

#endif // MESH_RENDERER_HPP
