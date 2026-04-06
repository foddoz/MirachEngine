#ifndef MESHSKYDOME_HPP
#define MESHSKYDOME_HPP

#include "mesh3d.hpp"
#include "../../Math/math.hpp"
#include <vector>
#include <string>

/**
 * @class MeshSkyDome
 * @brief A hemispherical skydome mesh used to render sky backgrounds.
 *
 * This class generates a dome-shaped mesh with configurable resolution and radius,
 * uploads it to GPU, and prepares it for ECS rendering. It supports texture mapping
 * for sky visuals (e.g., clouds or gradients).
 */
class MeshSkyDome : public Mesh3D
{
public:
    /**
     * @brief Constructs a MeshSkyDome with default initialization.
     */
    MeshSkyDome();

    /**
     * @brief Generates the dome's vertex and index data based on spherical subdivision.
     *
     * @param numRings Number of latitude subdivisions (rings).
     * @param numSectors Number of longitude subdivisions (sectors).
     * @param radius Radius of the hemisphere.
     * @param textureFile File path to the dome's sky texture.
     */
    void GenerateCPUData(int numRings, int numSectors, float radius, const std::string& textureFile);

    /**
     * @brief Uploads geometry and texture data from CPU to GPU.
     */
    void UploadToGPU() override;

    /**
     * @brief Converts internal mesh representation to a renderable ECS component.
     * @return MeshRenderer struct containing GPU handles and metadata.
     */
    MeshRenderer ToRenderer() override;

    /**
     * @brief Deletes GPU resources including VAO, VBO, and bound textures.
     */
    void Delete() override;

private:
    /**
     * @struct Vertex
     * @brief Represents a single skydome vertex with 3D position and 2D texture coordinates.
     */
    struct Vertex
    {
        Vector3f pos; ///< Vertex position in world space.
        Vector2f tex; ///< Texture coordinates (u, v).
    };

    /**
     * @brief Generates VAO, VBO, and IBO and uploads vertex/index data to the GPU.
     */
    void PopulateBuffers() override;

    int m_numRings;    ///< Number of vertical ring slices (latitude).
    int m_numSectors;  ///< Number of horizontal sector slices (longitude).
    float m_radius;    ///< Radius of the skydome.

    std::vector<Vertex> m_cpuVertexData;      ///< Vertex buffer stored on CPU.
    std::vector<unsigned int> m_cpuIndexData; ///< Index buffer stored on CPU.
    std::string m_textureFileName;             ///< Path to the texture image used for the skydome.
};

#endif // MESHSKYDOME_HPP
