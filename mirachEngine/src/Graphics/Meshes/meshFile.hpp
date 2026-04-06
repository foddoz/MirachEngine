#ifndef MESHFILE_HPP
#define MESHFILE_HPP

#include "../model_loading.hpp"
#include "mesh_entry.hpp"

#include <string>
#include "mesh3d.hpp" // Base class Mesh3D

/**
 * @class MeshFile
 * @brief Represents a static 3D mesh loaded from file (e.g. OBJ, FBX, etc.) using Assimp.
 *
 * Handles loading vertex and material data from external model files, converting them
 * into OpenGL-compatible buffers, and exposing rendering interfaces.
 */
class MeshFile : public Mesh3D
{
public:
    /**
     * @brief Constructs an empty MeshFile object.
     */
    MeshFile();

    /**
     * @brief Loads mesh geometry and material data from a model file into CPU memory.
     *
     * @param fileName Path to the mesh file (e.g. OBJ, FBX).
     */
    void GenerateCPUData(const std::string& fileName);

    /**
     * @brief Uploads vertex/index data and textures to GPU buffers.
     */
    void UploadToGPU() override;

    /**
     * @brief Converts internal mesh structure to a renderable ECS-compatible object.
     *
     * @return MeshRenderer data with GPU handles and metadata.
     */
    MeshRenderer ToRenderer() override;

    /**
     * @brief Frees GPU resources including VAO, VBOs, and texture memory.
     */
    void Delete() override;

private:
    /**
     * @enum BUFFER_TYPE
     * @brief Enum for identifying OpenGL buffer types used for this mesh.
     */
    enum BUFFER_TYPE
    {
        INDEX_BUFFER = 0,  ///< Element index buffer
        POS_VB = 1,        ///< Vertex position buffer
        TEXCOORD_VB = 2,   ///< Texture coordinate buffer
        NORMAL_VB = 3,     ///< Vertex normal buffer
        WVP_MAT_VB = 4,    ///< World-view-projection matrix buffer (optional)
        WORLD_MAT_VB = 5,  ///< World matrix buffer (optional)
        NUM_BUFFERS = 6    ///< Total number of buffer types
    };

    std::vector<MeshEntry> m_meshes; ///< Submeshes split by material or node.

    // === CPU-side geometry data ===
    std::vector<Vector3f> m_positions;   ///< Vertex positions
    std::vector<Vector3f> m_normals;     ///< Vertex normals
    std::vector<Vector2f> m_texCoords;   ///< Texture coordinates
    std::vector<unsigned int> m_indices; ///< Vertex indices

    GLuint m_Buffers[NUM_BUFFERS] = { 0 }; ///< OpenGL buffer handles

    /**
     * @brief Calculates total vertices and indices needed from an Assimp scene.
     *
     * @param pScene Pointer to Assimp's scene object.
     * @param numVertices Output: total vertex count.
     * @param numIndices Output: total index count.
     */
    void CountVerticesAndIndices(const ModelScene* pScene, unsigned int& numVertices, unsigned int& numIndices);

    /**
     * @brief Allocates space in vectors based on counted vertices and indices.
     *
     * @param numVertices Number of vertices to reserve.
     * @param numIndices Number of indices to reserve.
     */
    void ReserveSpace(unsigned int numVertices, unsigned int numIndices);

    /**
     * @brief Initializes all submeshes in the scene.
     *
     * @param pScene Pointer to the parsed scene from Assimp.
     */
    void InitAllMeshes(const ModelScene* pScene);

    /**
     * @brief Processes a single mesh and extracts geometry data.
     *
     * @param paiMesh Pointer to the mesh from Assimp.
     */
    void InitSingleMesh(const ModelMesh* paiMesh);

    /**
     * @brief Loads diffuse textures from materials.
     *
     * @param pScene Pointer to the Assimp scene.
     * @param fileName Path to the original mesh file for resolving texture directory.
     * @return true if material loading was successful.
     */
    bool InitMaterials(const ModelScene* pScene, const std::string& fileName);

    /**
     * @brief Creates OpenGL buffers and uploads mesh data to the GPU.
     */
    void PopulateBuffers() override;
};

#endif // MESHFILE_HPP
