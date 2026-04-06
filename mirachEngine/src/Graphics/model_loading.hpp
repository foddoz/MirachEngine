#ifndef MODEL_LOADING_HPP
#define MODEL_LOADING_HPP

// === Assimp ===
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <string>
#include "../Math/math.hpp"

// === Assimp Type Aliases ===
using ModelImporter = Assimp::Importer;
using ModelScene = aiScene;
using ModelMesh = aiMesh;
using ModelMaterial = aiMaterial;
using ModelFace = aiFace;

// === Assimp Load Flags ===
constexpr unsigned int modelLoadFlags =
aiProcess_Triangulate |
aiProcess_GenSmoothNormals |
aiProcess_FlipUVs |
aiProcess_JoinIdenticalVertices;

// === Assimp Helper Functions ===

// Load model from file
inline const ModelScene* LoadModel(ModelImporter& importer, const std::string& fileName)
{
    return importer.ReadFile(fileName, modelLoadFlags);
}

// Get material texture count
inline unsigned int GetMaterialTextureCount(const ModelMaterial* material)
{
    return material->GetTextureCount(aiTextureType_DIFFUSE);
}

// Get the name of the material (if present)
inline std::string GetMaterialName(const ModelMaterial* material)
{
    aiString name;
    if (material->Get(AI_MATKEY_NAME, name) == AI_SUCCESS)
    {
        return std::string(name.C_Str());
    }
    return "(Unnamed)";
}


// Get material texture path
inline bool GetMaterialTexturePath(const ModelMaterial* material, std::string& outPath)
{
    aiString path;
    if (material->GetTexture(aiTextureType_DIFFUSE, 0, &path) == AI_SUCCESS)
    {
        outPath = path.C_Str();
        return true;
    }
    return false;
}

// Accessor helpers for meshes
inline unsigned int GetNumVertices(const ModelMesh* mesh)
{
    return mesh->mNumVertices;
}

inline unsigned int GetNumFaces(const ModelMesh* mesh)
{
    return mesh->mNumFaces;
}

inline const Vector3f GetVertexPosition(const ModelMesh* mesh, unsigned int i)
{
    const aiVector3D& v = mesh->mVertices[i];
    return Vector3f(v.x, v.y, v.z);
}

inline const Vector3f GetVertexNormal(const ModelMesh* mesh, unsigned int i)
{
    const aiVector3D& n = mesh->mNormals[i];
    return Vector3f(n.x, n.y, n.z);
}

inline const Vector2f GetVertexTexCoord(const ModelMesh* mesh, unsigned int i)
{
    if (mesh->HasTextureCoords(0)) 
    {
        const aiVector3D& uv = mesh->mTextureCoords[0][i];
        return Vector2f(uv.x, uv.y); // Preserve z = 0.0 for 2D texcoords
    }
    else 
    {
        return Vector2f(0.0f, 0.0f);
    }
}

inline bool HasTextureCoords(const ModelMesh* mesh)
{
    return mesh->HasTextureCoords(0);
}

inline const ModelFace& GetFace(const ModelMesh* mesh, unsigned int i)
{
    return mesh->mFaces[i];
}

#endif // !MODEL_LOADING_HPP
