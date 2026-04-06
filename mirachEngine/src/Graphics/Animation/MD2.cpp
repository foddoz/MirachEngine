#include "MD2.hpp"
#include <fstream>
#include <cstring>

// Ensure the MD2Header structure is packed with no padding
#pragma pack(push, 1)
struct MD2Header 
{
    int32_t ident;
    int32_t version;
    int32_t skinWidth, skinHeight;
    int32_t frameSize;
    int32_t numSkins, numVertices, numTexCoords;
    int32_t numTriangles, numGLCmds, numFrames;
    int32_t offsetSkins, offsetTexCoords, offsetTriangles;
    int32_t offsetFrames, offsetGLCmds, offsetEnd;
};
#pragma pack(pop)

MeshMD2::MeshMD2()
    : 
    m_texture(nullptr)
{
}

MeshMD2::MeshMD2(const MeshMD2& other)
    : 
    Mesh3D(other),
    m_frames(other.m_frames),
    m_texCoords(other.m_texCoords),
    m_triangles(other.m_triangles),
    m_numVertices(other.m_numVertices),
    m_skinWidth(other.m_skinWidth),
    m_skinHeight(other.m_skinHeight),
    m_interpolatedVertices(other.m_interpolatedVertices),
    m_animations(other.m_animations),
    m_uvs(other.m_uvs)
{
    // Reset OpenGL IDs since buffers are not shared
    m_vao = 0;
    m_vbo = 0;
    m_uv = 0;
    m_indexCount = 0;

    // Deep copy the texture if it exists
    if (other.m_texture) 
    {
        m_texture = new Texture(*other.m_texture);
    }
    else 
    {
        m_texture = nullptr;
    }
}

bool MeshMD2::LoadFromFile(const std::string& modelFile, const std::string& textureFile) 
{
    std::ifstream file(modelFile, std::ios::binary);
    if (!file)
    {
        return false;
    }

    // Read MD2 header and validate format
    MD2Header header;
    file.read(reinterpret_cast<char*>(&header), sizeof(MD2Header));
    if (header.ident != ('I' | ('D' << 8) | ('P' << 16) | ('2' << 24)) || header.version != 8)
    {
        return false;
    }

    // Store basic model metadata
    m_numVertices = header.numVertices;
    m_skinWidth = header.skinWidth;
    m_skinHeight = header.skinHeight;

    // Load texture coordinates
    file.seekg(header.offsetTexCoords);
    m_texCoords.resize(header.numTexCoords);
    file.read(reinterpret_cast<char*>(m_texCoords.data()), sizeof(MD2TexCoord) * header.numTexCoords);

    // Load triangle indices
    file.seekg(header.offsetTriangles);
    m_triangles.resize(header.numTriangles);
    file.read(reinterpret_cast<char*>(m_triangles.data()), sizeof(MD2Triangle) * header.numTriangles);

    // Load animation frames
    m_frames.resize(header.numFrames);
    file.seekg(header.offsetFrames);
    for (int i = 0; i < header.numFrames; ++i) 
    {
        float scale[3], translate[3];
        char name[16];
        file.read(reinterpret_cast<char*>(scale), sizeof(float) * 3);
        file.read(reinterpret_cast<char*>(translate), sizeof(float) * 3);
        file.read(reinterpret_cast<char*>(name), 16);

        // Decompress vertices
        m_frames[i].vertices.resize(header.numVertices);
        for (int j = 0; j < header.numVertices; ++j) 
        {
            MD2Vertex v;
            file.read(reinterpret_cast<char*>(&v), sizeof(MD2Vertex));
            m_frames[i].vertices[j] = Vector3f(
                v.v[0] * scale[0] + translate[0],
                v.v[1] * scale[1] + translate[1],
                v.v[2] * scale[2] + translate[2]
            );
        }

        // Store raw transform data
        std::memcpy(m_frames[i].scale, scale, sizeof(float) * 3);
        std::memcpy(m_frames[i].translate, translate, sizeof(float) * 3);
        std::memcpy(m_frames[i].name, name, 16);
    }

    // Auto-detect animation frame ranges based on name prefixes
    std::string lastPrefix;
    int start = 0;
    for (int i = 0; i < m_frames.size(); ++i) 
    {
        std::string name(m_frames[i].name);
        std::string prefix = ExtractPrefix(name);
        if (prefix != lastPrefix && !lastPrefix.empty()) 
        {
            m_animations[lastPrefix] = { start, i - 1 };
            start = i;
        }
        lastPrefix = prefix;
    }

    // Register the final animation range
    if (!lastPrefix.empty()) 
    {
        m_animations[lastPrefix] = { start, static_cast<int>(m_frames.size()) - 1 };
    }

    // Print available animation groups
    /*
    std::cout << "Available animations:\n";
    for (const auto& [key, range] : animations) {
        std::cout << "  " << key << ": " << range.startFrame << " to " << range.endFrame << "\n";
    }
    */
    // Load texture
    m_texture = new Texture(TextureTarget::Texture2D);
    m_texture->LoadFromFile(textureFile);

    // Set default AABB using "stand" animation
    std::string defaultAnimation = "stand";
    auto it = m_animations.find(defaultAnimation);
    if (it != m_animations.end()) 
    {
        const AnimationRange& range = it->second;
        for (int i = range.startFrame; i <= range.endFrame; ++i) 
        {
            const auto& frame = m_frames[i];
            //std::cout << "Frame " << i << " name: " << frame.name << "\n";
            for (const auto& vertex : frame.vertices) 
            {
                m_AABB.Expand(vertex);
            }
        }
    }
    else 
    {
        std::cerr << "Default animation not found: " << defaultAnimation << "\n";
    }

    return true;
}

void MeshMD2::UploadToGPU() 
{
    PopulateBuffers();
}

void MeshMD2::PopulateBuffers() 
{
    m_interpolatedVertices.clear();
    m_uvs.clear();

    // Build vertex/UV arrays using the first frame only
    for (const auto& tri : m_triangles) 
    {
        for (int i = 0; i < 3; ++i) 
        {
            int vi = tri.vertexIndices[i];
            int ti = tri.texCoordIndices[i];

            if (vi < m_frames[0].vertices.size() && ti < m_texCoords.size()) 
            {
                m_interpolatedVertices.push_back(m_frames[0].vertices[vi]);

                // Normalize texture coordinates to [0,1]
                float s = m_texCoords[ti].s / static_cast<float>(m_skinWidth);
                float t = 1.0f - (m_texCoords[ti].t / static_cast<float>(m_skinHeight));
                m_uvs.push_back(Vector2f(s, t));
            }
        }
    }

    m_numVertices = static_cast<int>(m_interpolatedVertices.size());

    // Generate and bind VAO and VBOs
    Graphics::GenVertexArray(m_vao);
    Graphics::GenBuffer(m_vbo);
    Graphics::GenBuffer(m_uv);

    Graphics::BindVertexArray(m_vao);

    // Upload vertex positions
    Graphics::BindBuffer(BufferTarget::ArrayBuffer, m_vbo);
    Graphics::BufferData(BufferTarget::ArrayBuffer, sizeof(Vector3f) * m_numVertices, m_interpolatedVertices.data(), BufferUsage::DynamicDraw);
    Graphics::SetVertexAttribPointer(POSITION_LOCATION, 3, DataType::Float, false, sizeof(Vector3f), (void*)0);
    Graphics::EnableVertexAttrib(POSITION_LOCATION);

    // Upload UV coordinates
    Graphics::BindBuffer(BufferTarget::ArrayBuffer, m_uv);
    Graphics::BufferData(BufferTarget::ArrayBuffer, sizeof(Vector2f) * m_uvs.size(), m_uvs.data(), BufferUsage::StaticDraw);
    Graphics::SetVertexAttribPointer(TEX_COORD_LOCATION, 2, DataType::Float, false, sizeof(Vector2f), (void*)0);
    Graphics::EnableVertexAttrib(TEX_COORD_LOCATION);

    Graphics::UnbindVertexArray();

    // Upload texture to GPU
    m_texture->UploadToGPU();

    // Store vertex count for draw calls (using glDrawArrays)
    m_indexCount = m_numVertices;
}

void MeshMD2::SetAnimation(const std::string& name) 
{
    if (m_animations.find(name) == m_animations.end()) 
    {
        std::cerr << "Animation not found: " << name << "\n";
    }
}

MeshRenderer MeshMD2::ToRenderer() 
{
    MeshRenderer renderer;

    MeshLOD lod;
    lod.vao = m_vao;
    lod.vbo = m_vbo;
    lod.ibo = 0; // Not using indexed drawing
    lod.indexCount = m_indexCount;
    lod.localAABB = m_AABB;

    renderer.lods.push_back(lod);
    renderer.pipeline = m_pipeline;
    renderer.textures.push_back(m_texture);
    renderer.textureUniformNames.push_back("uTexture");

    return renderer;
}

const AnimationRange& MeshMD2::GetAnimationRange(const std::string& name) const 
{
    static AnimationRange dummy{ 0, 0 };
    auto it = m_animations.find(name);
    return it != m_animations.end() ? it->second : dummy;
}

void MeshMD2::Interpolate(int frameA, int frameB, float blendAlpha, std::vector<Vector3f>& outVertices) const 
{
    outVertices.resize(m_triangles.size() * 3);

    const auto& vertsA = m_frames[frameA].vertices;
    const auto& vertsB = m_frames[frameB].vertices;

    // Lerp each triangle vertex between two frames
    for (size_t i = 0; i < m_triangles.size(); ++i) 
    {
        for (int j = 0; j < 3; ++j) 
        {
            int vi = m_triangles[i].vertexIndices[j];
            const Vector3f& v0 = vertsA[vi];
            const Vector3f& v1 = vertsB[vi];

            outVertices[i * 3 + j] = Lerp(v0, v1, blendAlpha);
        }
    }
}

int MeshMD2::GetVertexCount() const 
{
    return m_numVertices;
}

std::string MeshMD2::ExtractPrefix(const std::string& name) 
{
    // Trim trailing digits from the name to extract the animation group prefix
    size_t lastNonDigit = name.find_last_not_of("0123456789");
    if (lastNonDigit == std::string::npos)
    {
        return name; // fallback if name is all digits
    }
    return name.substr(0, lastNonDigit + 1);
}
