#pragma once

#include <iostream>
#include <vector>
#include <string>
#include <cstdint>
#include <unordered_map>

#include "../../Math/math.hpp"     // Contains Vector3f, Vector2f
#include "../graphics.hpp"
#include "../Meshes/mesh3d.hpp"
#include "../Texturing/texture.hpp"

/**
 * @struct MD2Vertex
 * @brief Represents a compressed vertex in MD2 format with quantized position and normal index.
 */
struct MD2Vertex 
{
    uint8_t v[3];          ///< Quantized vertex position.
    uint8_t normalIndex;   ///< Index into MD2 normal table.
};

/**
 * @struct MD2Frame
 * @brief Stores a single animation frame's vertex positions in MD2.
 */
struct MD2Frame 
{
    float scale[3];                    ///< Scale applied to each axis.
    float translate[3];                ///< Translation vector applied to each axis.
    char name[16];                     ///< Frame name (often includes animation prefix).
    std::vector<Vector3f> vertices;    ///< Decompressed vertex positions for this frame.
};

/**
 * @struct MD2TexCoord
 * @brief Represents a texture coordinate in MD2 format.
 */
struct MD2TexCoord 
{
    int16_t s, t;   ///< Texture coordinates (usually in pixels).
};

/**
 * @struct MD2Triangle
 * @brief Represents a single triangle using vertex and texture coordinate indices.
 */
struct MD2Triangle 
{
    uint16_t vertexIndices[3];     ///< Indices into frame vertex arrays.
    uint16_t texCoordIndices[3];   ///< Indices into texture coordinate array.
};

/**
 * @struct AnimationRange
 * @brief Describes the start and end frame indices for a named animation sequence.
 */
struct AnimationRange 
{
    int startFrame;  ///< Index of first frame in the animation.
    int endFrame;    ///< Index of last frame in the animation.
};

/**
 * @class MeshMD2
 * @brief A 3D animated mesh class for rendering MD2 models with frame interpolation.
 *
 * Inherits from Mesh3D. Supports loading MD2 models, playing animations by name,
 * and interpolating between frames for smooth animation playback.
 */
class MeshMD2 : public Mesh3D 
{
public:
    /**
     * @brief Default constructor.
     */
    MeshMD2();

    /**
     * @brief Copy constructor.
     * @param other The mesh to copy.
     */
    MeshMD2(const MeshMD2& other);

    /**
     * @brief Loads an MD2 model and its texture from file.
     * @param modelFile Path to the MD2 model file.
     * @param textureFile Path to the texture image file.
     * @return True if loaded successfully.
     */
    bool LoadFromFile(const std::string& modelFile, const std::string& textureFile);

    /**
     * @brief Uploads the current mesh data to the GPU.
     */
    void UploadToGPU() override;

    /**
     * @brief Sets the active animation by name.
     * @param name The animation name.
     */
    void SetAnimation(const std::string& name);

    /**
     * @brief Gets the frame range of a named animation.
     * @param name Animation name.
     * @return The AnimationRange corresponding to the name.
     */
    const AnimationRange& GetAnimationRange(const std::string& name) const;

    /**
     * @brief Interpolates between two animation frames to produce smooth vertex output.
     * @param frameA Index of the first frame.
     * @param frameB Index of the second frame.
     * @param blendAlpha Interpolation factor [0.0, 1.0].
     * @param outVertices Output array of interpolated vertices.
     */
    void Interpolate(int frameA, int frameB, float blendAlpha, std::vector<Vector3f>& outVertices) const;

    /**
     * @brief Gets the total number of vertices in the mesh.
     * @return Vertex count.
     */
    int GetVertexCount() const;

    /**
     * @brief Converts the mesh into a MeshRenderer struct for ECS rendering.
     * @return The MeshRenderer representation.
     */
    MeshRenderer ToRenderer() override;

private:
    std::vector<MD2Frame> m_frames;                ///< All animation frames.
    std::vector<MD2TexCoord> m_texCoords;          ///< Texture coordinate list.
    std::vector<MD2Triangle> m_triangles;          ///< Triangle definitions (indexed geometry).

    int m_numVertices;                             ///< Total number of vertices.
    int m_skinWidth;
    int m_skinHeight;                 ///< Texture dimensions in pixels.

    /**
     * @brief Fills OpenGL buffers with mesh data (called from UploadToGPU).
     */
    void PopulateBuffers() override;

    /**
     * @brief Extracts the animation name prefix from a frame name.
     * @param name The full frame name (e.g., "run01").
     * @return The extracted prefix (e.g., "run").
     */
    std::string ExtractPrefix(const std::string& name);

    std::vector<Vector3f> m_interpolatedVertices;  ///< Working buffer for interpolated vertex positions.

    std::unordered_map<std::string, AnimationRange> m_animations; ///< Named animation ranges parsed from frame names.

    Texture* m_texture;                              ///< Associated texture (diffuse map).

    unsigned int m_uv = 0;                          ///< OpenGL UV buffer object (VBO).
    std::vector<Vector2f> m_uvs;                   ///< UV coordinates per vertex.
};

