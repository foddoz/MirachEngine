#ifndef MESHTERRAIN_HPP
#define MESHTERRAIN_HPP

#include "mesh3d.hpp"

#include "../../Math/math.hpp"

#include <vector>
#include <iostream>

/**
 * @brief Terrain mesh class using heightmap generation.
 *
 * Generates and manages terrain data using a fault formation algorithm and uploads the mesh to GPU.
 */
class MeshTerrain : public Mesh3D
{
public:
    /**
     * @brief Constructs an empty MeshTerrain object.
     */
    MeshTerrain();

    /**
     * @brief Generates the heightmap and vertex data for terrain mesh.
     *
     * @param terrainSize Width and depth of the terrain grid.
     * @param iterations Number of fault iterations.
     * @param minHeight Minimum height value.
     * @param maxHeight Maximum height value.
     * @param filter FIR filter strength for smoothing.
     * @param textureScale Multiplier for texture UV mapping.
     * @param textureFileNames List of texture file paths for the terrain layers.
     */
    void GenerateCPUData(int terrainSize, int iterations, float minHeight, float maxHeight, float filter, float textureScale, const std::vector<std::string>& textureFileNames, bool randomise = true);

    /**
     * @brief Internal fault formation algorithm with smoothing filter.
     */
    void CreateFaultFormation(int iterations, float minHeight, float maxHeight, float filter, int terrainSize, bool randomise);

    /**
     * @brief Uploads generated mesh data and textures to the GPU.
     */
    void UploadToGPU() override;

    /**
     * @brief Converts the terrain to a renderable mesh component.
     *
     * @return A MeshRenderer component for use in ECS rendering.
     */
    MeshRenderer ToRenderer() override;

    MeshLOD CreateLOD(int stepSize);

    /**
     * @brief Gets the height at the given integer terrain grid coordinates.
     */
    float GetHeight(int x, int z) const;

    float GetWorldHeight(float x, float z) const;

    /**
     * @brief Gets bilinearly interpolated height from floating point coordinates.
     */
    float GetHeightInterpolated(float x, float z) const;

    /**
     * @brief Gets terrain grid size.
     */
    int GetSize() const;

    /**
     * @brief Gets the width (x-size) of the terrain.
     */
    int GetWidth() const;

    /**
     * @brief Gets the depth (z-size) of the terrain.
     */
    int GetDepth() const;

    float GetWorldWidth() const;

    float GetWorldDepth() const;

    int GetMinHeight() const;

    int GetMaxHeight() const;

    //const std::vector<float>& GetHeightMap() const { return m_heightMap; }
    const std::vector<float>& GetHeightMap() const;

    struct Vertex
    {
        Vector3f pos;     ///< Vertex position in world space.
        Vector2f tex;     ///< Texture coordinate.
        Vector3f normal;  ///< Normal for lighting.

        /**
         * @brief Initializes vertex based on heightmap and terrain data.
         */
        void InitVertex(int x, int z, const MeshTerrain* terrain);
    };

    const std::vector<MeshTerrain::Vertex>& GetVertexData() const { return m_cpuVertexData; }
    const std::vector<unsigned int>& GetIndexData() const { return m_cpuIndexData; }

    void ClearCPUData();

    /**
     * @brief Gets the texture UV scale factor.
     */
    float GetTextureScale() const;

    /**
     * @brief Deletes GPU resources associated with the terrain.
     */
    void Delete() override;

protected:
    /**
     * @brief Generates vertices, normals, and triangle indices from heightmap.
     */
    void CreateTriangleList(int width, int depth);

    void SetHeight(int x, int z, float value);

    std::vector<std::string> m_textureFileNames;     ///< Texture file names for terrain layers.

    int m_width;     ///< Final mesh width after generation.
    int m_depth;     ///< Final mesh depth after generation.
    int m_minHeight; ///< Cached minimum height value.
    int m_maxHeight; ///< Cached maximum height value.

    float m_worldSizeX;
    float m_worldSizeZ;

    int m_terrainSize;                    ///< Size of the terrain (width = depth).
    std::vector<float> m_heightMap;          ///< 2D heightmap values.
    float m_textureScale;                ///< Texture scale used for UV mapping.

private:
    /**
     * @brief Structure representing a vertex on the terrain mesh.
     */

    /**
     * @brief Point representation for fault generation.
     */
    struct TerrainPoint
    {
        int x = 0;
        int z = 0;

        void Print()
        {
            std::cout << x << ", " << z << "\n";
        }

        bool isEqual(TerrainPoint& p) const
        {
            return ((x == p.x) && (z == p.z));
        }
    };

    /**
     * @brief Fills index buffer with triangle strip indices.
     */
    void InitIndices(std::vector<unsigned int>& indices, int width, int depth);

    /**
     * @brief Allocates OpenGL VAO/VBO/IBO and sets vertex format.
     */
    void PopulateBuffers() override;

    /**
     * @brief Initializes vertex positions and texture coordinates.
     */
    void InitVertices(std::vector<Vertex>& Vertices);

    /**
     * @brief Computes per-vertex normals by averaging surrounding triangles.
     */
    //void CalcNormals(std::vector<Vertex>& Vertices);
    void CalcNormals(std::vector<Vertex>& Vertices, int width, int depth);

    /**
     * @brief Generates two random points for a fault line.
     */
    void GenRandomTerrainPoints(TerrainPoint& p1, TerrainPoint& p2);

    /**
     * @brief Applies FIR filter in 4 directions for terrain smoothing.
     */
    void ApplyFIRFilter(float filter);

    /**
     * @brief Applies FIR filtering on a single height value.
     */
    float FIRFilterSinglePoint(int x, int z, float prevVal, float filter);

    void NormaliseHeightMap(float minHeight, float maxHeight);

    std::vector<Vertex> m_cpuVertexData;            ///< Vertices generated on CPU.
    std::vector<unsigned int> m_cpuIndexData;       ///< Indices for terrain triangles.
};

#endif // MESHTERRAIN_HPP
