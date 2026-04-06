#include <vector>
#include <iostream>

#include "meshFile.hpp"

// Constructor for MeshFile, calls base Mesh3D constructor
MeshFile::MeshFile()
    : 
    Mesh3D()
{
}

// Loads mesh from file and prepares all CPU-side data
void MeshFile::GenerateCPUData(const std::string& fileName)
{
    ModelImporter importer;
    const ModelScene* pScene = LoadModel(importer, fileName);

    if (!pScene)
    {
        std::cerr << "Error loading mesh.\n";
        exit(1);
    }

    m_AABB = AABB();
    m_meshes.resize(pScene->mNumMeshes);
    m_textures.resize(pScene->mNumMaterials);

    unsigned int numVertices = 0, numIndices = 0;

    CountVerticesAndIndices(pScene, numVertices, numIndices);
    ReserveSpace(numVertices, numIndices);
    InitAllMeshes(pScene);

    if (!InitMaterials(pScene, fileName))
    {
        std::cerr << "Failed loading materials\n";
        exit(1);
    }
}

// Loop through meshes to determine total vertex/index count
void MeshFile::CountVerticesAndIndices(const ModelScene* pScene, unsigned int& numVertices, unsigned int& numIndices)
{
    for (unsigned int i = 0; i < m_meshes.size(); i++)
    {
        m_meshes[i].materialIndex = pScene->mMeshes[i]->mMaterialIndex;
        m_meshes[i].numIndices = GetNumFaces(pScene->mMeshes[i]) * 3;
        m_meshes[i].baseVertex = numVertices;
        m_meshes[i].baseIndex = numIndices;

        numVertices += GetNumVertices(pScene->mMeshes[i]);
        numIndices += m_meshes[i].numIndices;
    }
}

// Preallocate space for vertex/index buffers
void MeshFile::ReserveSpace(unsigned int numVertices, unsigned int numIndices)
{
    m_positions.reserve(numVertices);
    m_normals.reserve(numVertices);
    m_texCoords.reserve(numVertices);
    m_indices.reserve(numIndices);
}

// Initialize all meshes found in the scene
void MeshFile::InitAllMeshes(const ModelScene* pScene)
{
    for (unsigned int i = 0; i < m_meshes.size(); i++)
    {
        const ModelMesh* mesh = pScene->mMeshes[i];
        unsigned int matIndex = mesh->mMaterialIndex;
        //std::cout << "Mesh " << i << " uses material index " << matIndex << " (" << GetMaterialName(pScene->mMaterials[matIndex]) << ")\n";

        InitSingleMesh(mesh);
    }
}

// Fill vertex/index data for a single mesh
void MeshFile::InitSingleMesh(const ModelMesh* mesh)
{
    for (unsigned int i = 0; i < GetNumVertices(mesh); i++)
    {
        const Vector3f pPos = GetVertexPosition(mesh, i);
        const Vector3f pNormal = GetVertexNormal(mesh, i);
        const Vector2f pTexCoord = GetVertexTexCoord(mesh, i);

        //Vector3f pos(pPos.x, pPos.y, pPos.z);
        m_positions.push_back(pPos);
        m_normals.push_back(Vector3f(pNormal));
        m_texCoords.push_back(Vector2f(pTexCoord));

        m_AABB.Expand(pPos);
    }

    for (unsigned int i = 0; i < GetNumFaces(mesh); i++)
    {
        const ModelFace& face = GetFace(mesh, i);
        assert(face.mNumIndices == 3);

        m_indices.push_back(face.mIndices[0]);
        m_indices.push_back(face.mIndices[1]);
        m_indices.push_back(face.mIndices[2]);
    }
}

// Load materials and textures used by the model
bool MeshFile::InitMaterials(const ModelScene* pScene, const std::string& fileName)
{
    std::string::size_type slashIndex = fileName.find_last_of("/");
    std::string dir;

    if (slashIndex == std::string::npos)
    {
        dir = ".";
    }
    else if (slashIndex == 0)
    {
        dir = "/";
    }
    else
    {
        dir = fileName.substr(0, slashIndex);
    }

    bool ret = true;

    for (unsigned int i = 0; i < pScene->mNumMaterials; i++)
    {
        const ModelMaterial* material = pScene->mMaterials[i];
        std::string name = GetMaterialName(material);  // implement this if not present
        //std::cout << "Material index: " << i << ", name: " << name << "\n";
        m_textures[i] = nullptr;

        if (GetMaterialTextureCount(material) > 0)
        {
            std::string path;
            if (GetMaterialTexturePath(material, path))
            {
                //std::cout << "Material " << i << " diffuse texture path: " << path << "\n";
                std::string p = path;
                if (p.substr(0, 2) == ".\\")
                {
                    p = p.substr(2);
                }

                std::string fullPath = dir + "/" + p;
                std::replace(fullPath.begin(), fullPath.end(), '\\', '/');

                m_textures[i] = new Texture(TextureTarget::Texture2D);
                m_textures[i]->LoadFromFile(fullPath.c_str());
            }
        }
    }

    return ret;
}


// Create and upload GPU buffers
void MeshFile::UploadToGPU()
{
    using namespace Graphics;

    GenVertexArray(m_vao);
    BindVertexArray(m_vao);

    // Generate each buffer in m_Buffers array
    for (int i = 0; i < NUM_BUFFERS; ++i) 
    {
        GenBuffer(m_Buffers[i]);
    }

    PopulateBuffers();

    for (auto& tex : m_textures) 
    {
        if (tex)
        {
            tex->UploadToGPU();
        }
    }

    UnbindVertexArray();
}

// Upload all buffer data to GPU
void MeshFile::PopulateBuffers()
{
    using namespace Graphics;

    // Position buffer
    BindBuffer(BufferTarget::ArrayBuffer, m_Buffers[POS_VB]);
    BufferData(BufferTarget::ArrayBuffer, sizeof(m_positions[0]) * m_positions.size(), m_positions.data(), BufferUsage::StaticDraw);
    EnableVertexAttrib(POSITION_LOCATION);
    SetVertexAttribPointer(POSITION_LOCATION, 3, DataType::Float, false, 0, nullptr);

    // Texcoord buffer
    BindBuffer(BufferTarget::ArrayBuffer, m_Buffers[TEXCOORD_VB]);
    BufferData(BufferTarget::ArrayBuffer, sizeof(m_texCoords[0]) * m_texCoords.size(), m_texCoords.data(), BufferUsage::StaticDraw);
    EnableVertexAttrib(TEX_COORD_LOCATION);
    SetVertexAttribPointer(TEX_COORD_LOCATION, 2, DataType::Float, false, 0, nullptr);

    // Normal buffer
    BindBuffer(BufferTarget::ArrayBuffer, m_Buffers[NORMAL_VB]);
    BufferData(BufferTarget::ArrayBuffer, sizeof(m_normals[0]) * m_normals.size(), m_normals.data(), BufferUsage::StaticDraw);
    EnableVertexAttrib(NORMAL_LOCATION);
    SetVertexAttribPointer(NORMAL_LOCATION, 3, DataType::Float, false, 0, nullptr);

    // Index buffer
    BindBuffer(BufferTarget::ElementArrayBuffer, m_Buffers[INDEX_BUFFER]);
    BufferData(BufferTarget::ElementArrayBuffer, sizeof(m_indices[0]) * m_indices.size(), m_indices.data(), BufferUsage::StaticDraw);

}

// Free all GPU resources and textures
void MeshFile::Delete()
{
    Graphics::DeleteVertexArray(m_vao);

    for (int i = 0; i < NUM_BUFFERS; ++i) 
    {
        Graphics::DeleteBuffer(m_Buffers[i]);
    }

    for (auto& texture : m_textures) 
    {
        if (texture) 
        {
            texture->Delete();
            delete texture;
            texture = nullptr;
        }
    }

    m_textures.clear();
    m_vao = 0;
    std::fill(std::begin(m_Buffers), std::end(m_Buffers), 0);
}

// Convert MeshFile into a MeshRenderer for use with ECS or renderer
MeshRenderer MeshFile::ToRenderer()
{
    MeshRenderer renderer;

    MeshLOD lod;
    lod.vao = m_vao;
    lod.vbo = m_Buffers[POS_VB];
    lod.ibo = m_Buffers[INDEX_BUFFER];
    lod.indexCount = 0; // Full model with submeshes, index counts inside m_meshes
    lod.localAABB = m_AABB;

    renderer.lods.push_back(lod);

    renderer.pipeline = m_pipeline;
    renderer.textures = m_textures;
    renderer.minHeight = 0.0f;
    renderer.maxHeight = 0.0f;
    renderer.textureUniformNames.push_back("gSampler");

    renderer.subMeshes = m_meshes;

    return renderer;
}
