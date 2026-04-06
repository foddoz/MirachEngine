#include "render_system.hpp"

#include "../../Transform/transform.hpp"
#include "../../ECS/component_manager.hpp"
#include "../../ECS/entity_manager.hpp"
#include "../../Camera/camera.hpp"
#include "../Animation/animation_system.hpp"

#include <iostream>

void RenderSkybox(const Program& gApp, const Camera& cam, const Transform& transform, const MeshRenderer& renderer)
{
    // Create skybox view matrix by removing translation (keep only rotation)
    Matrix4f view = cam.GetViewMatrix(transform.GetPosition());
    Matrix4f projection = cam.GetProjectionMatrix();
    Matrix4f skyView = Matrix4f(Matrix3f(view)); // strip translation
    Matrix4f wvp = projection * skyView * transform.GetModelMatrix();

    if (renderer.lods.empty()) 
    {
        std::cerr << "[RenderSkybox] ERROR: No LODs available for Skybox!\n";
        return;
    }

    const MeshLOD& lod = renderer.lods[renderer.currentLOD];

    if (renderer.pipeline == 0) 
    {
        std::cerr << "[RenderSkybox] ERROR: No valid shader pipeline for Skybox!\n";
        return;
    }

    // Disable backface culling and depth writing for background
    Graphics::Disable(Capability::CullFace);
    Graphics::Enable(Capability::DepthTest);
    Graphics::SetDepthWrite(false);

    Graphics::UseProgram(renderer.pipeline);

    // Upload WVP matrix
    int loc = Graphics::GetUniformLocation(renderer.pipeline, "gWVP");
    if (loc >= 0)
    {
        Graphics::SetUniformMat4(loc, &wvp[0][0]);
    }

    // Bind skybox texture
    if (!renderer.textures.empty() && renderer.textures[0]) 
    {
        renderer.textures[0]->Bind(COLOR_TEXTURE_UNIT);
        int samplerLoc = Graphics::GetUniformLocation(renderer.pipeline, "gSampler");
        if (samplerLoc >= 0)
        {
            Graphics::SetUniform1i(samplerLoc, 0);
        }
    }

    // Draw skybox
    Graphics::BindVertexArray(lod.vao);
    Graphics::DrawElements(DrawMode::Triangles, lod.indexCount, DataType::UnsignedInt, nullptr);
    Graphics::UnbindVertexArray();
    Graphics::UnbindProgram();

    // Restore OpenGL state
    Graphics::Enable(Capability::CullFace);
    Graphics::SetDepthWrite(true);
}

void RenderSystem(const Program& gApp, const Entity& camEntity, EntityManager& entityManager)
{
    using namespace Graphics;

    // === Global OpenGL State Setup ===
    Enable(Capability::CullFace);
    SetFrontFace(FrontFaceOrder::CCW);
    SetCullFace(CullFaceMode::Back);
    Enable(Capability::DepthTest);

    // Set viewport and clear screen
    Viewport(0, 0, gApp.GetScreenWidth(), gApp.GetScreenHeight());
    ClearColor(0.f, 1.f, 1.f, 1.f); // Cyan clear color
    Clear(BufferBit::Color | BufferBit::Depth);

    // === Camera Setup ===
    Camera& cam = entityManager.GetComponent<Camera>(camEntity);
    Transform& playerTF = entityManager.GetComponent<Transform>(camEntity);

    Matrix4f view = cam.GetViewMatrix(playerTF.GetPosition());
    Matrix4f projection = cam.GetProjectionMatrix();

    // === Sunlight Direction ===
    static float sunAngle = 90.0f;
    Vector3f baseSunDir = Vector3f(0.0f, 0.0f, 1.0f);
    Matrix4f sunRotation = Rotate(Matrix4f(1.0f), sunAngle, Vector3f(1.0f, 0.0f, 0.0f));
    Vector3f sunDir = Normalize(Vector3f(sunRotation * Vector4f(baseSunDir, 0.0f)));

    // === Main Mesh Loop ===
    for (auto& [entity, renderer] : entityManager.GetAllComponents<MeshRenderer>()) 
    {
        if (!entityManager.HasComponent<Transform>(entity))
        {
            continue;
        }
        if (renderer.lods.empty()) 
        {
            std::cerr << "ERROR: No LODs for entity " << entity << "\n";
            continue;
        }
        if (renderer.pipeline == 0) 
        {
            std::cerr << "ERROR: No shader for entity " << entity << "\n";
            continue;
        }

        const Transform& transform = entityManager.GetComponent<Transform>(entity);

        // Special case: Skybox entity (ID 1)
        if (entity == 1) 
        {
            RenderSkybox(gApp, cam, transform, renderer);
            continue;
        }

        Matrix4f model = transform.GetModelMatrix();
        Matrix4f wvp = projection * view * model;

        AABB worldAABB = renderer.lods[0].localAABB.Transform(model);
        if (!cam.IsBoxInFrustum(worldAABB.min, worldAABB.max))
        {
            continue; // Frustum culling
        }

        // === LOD Selection ===
        Vector3f center = (worldAABB.min + worldAABB.max) * 0.5f;
        float dist = Length(center - playerTF.GetPosition());
        int lod = 0;
        if (renderer.lods.size() > 2 && dist > 750.f)
        {
            lod = 2;
        }
        else if (renderer.lods.size() > 1 && dist > 500.f)
        {
            lod = 1;
        }
        renderer.currentLOD = std::clamp(lod, 0, int(renderer.lods.size() - 1));

        const MeshLOD& mesh = renderer.lods[renderer.currentLOD];

        UseProgram(renderer.pipeline);

        // Upload per-object uniforms
        int loc = GetUniformLocation(renderer.pipeline, "gWVP");
        if (loc >= 0)
        {
            SetUniformMat4(loc, &wvp[0][0]);
        }

        if ((loc = GetUniformLocation(renderer.pipeline, "gMinHeight")) >= 0)
        {
            SetUniform1f(loc, renderer.minHeight);
        }

        if ((loc = GetUniformLocation(renderer.pipeline, "gMaxHeight")) >= 0)
        {
            SetUniform1f(loc, renderer.maxHeight);
        }

        if ((loc = GetUniformLocation(renderer.pipeline, "gLightDir")) >= 0)
        {
            SetUniform3f(loc, sunDir.x, sunDir.y, sunDir.z);
        }
        if ((loc = GetUniformLocation(renderer.pipeline, "gLightColor")) >= 0)
        {
            SetUniform3f(loc, 1.0f, 1.0f, 1.0f);
        }

        // === Bind Textures ===
        if (renderer.subMeshes.empty()) 
        {
            for (int i = 0; i < renderer.textures.size(); ++i) 
            {
                if (!renderer.textures[i]) 
                {
                    std::cerr << "Null texture index " << i << " for entity " << entity << "\n";
                    continue;
                }
                renderer.textures[i]->Bind(COLOR_TEXTURE_UNIT + i);
                int samplerLoc = GetUniformLocation(renderer.pipeline, renderer.textureUniformNames[i]);
                if (samplerLoc >= 0) SetUniform1i(samplerLoc, i);
            }
        }

        // === Draw Mesh ===
        BindVertexArray(mesh.vao);

        // Submesh rendering (e.g. from file meshes with materials)
        if (!renderer.subMeshes.empty()) 
        {
            for (const auto& sub : renderer.subMeshes) 
            {
                if (sub.numIndices == 0)
                {
                    continue;
                }
                if (sub.materialIndex >= renderer.textures.size()) 
                {
                    std::cerr << "Invalid material index " << sub.materialIndex << "\n";
                    continue;
                }

                Texture* tex = renderer.textures[sub.materialIndex];
                if (!tex) 
                {
                    std::cerr << "Missing texture for material index " << sub.materialIndex << "\n";
                    continue;
                }

                tex->Bind(COLOR_TEXTURE_UNIT);
                int loc = GetUniformLocation(renderer.pipeline, "gSampler");
                if (loc >= 0)
                {
                    SetUniform1i(loc, 0);
                }

                if (mesh.vao == 0)
                {
                    continue;
                }
                if (GetCurrentVertexArray() != mesh.vao) 
                {
                    std::cerr << "VAO mismatch on entity " << entity << "\n";
                    continue;
                }

                // Draw using submesh data
                DrawElementsBaseVertex(
                    DrawMode::Triangles,
                    sub.numIndices,
                    DataType::UnsignedInt,
                    reinterpret_cast<void*>(sizeof(unsigned int) * sub.baseIndex),
                    sub.baseVertex
                );
            }
        }
        // Animated model (MD2)
        else if (entityManager.HasComponent<MD2Animator>(entity)) 
        {
            const auto& anim = entityManager.GetComponent<MD2Animator>(entity);

            OverridePositionVBO(anim.animatedVBO);

            // Reverse winding for inside-out animation fix
            SetFrontFace(FrontFaceOrder::CW);
            DrawArrays(DrawMode::Triangles, 0, mesh.indexCount);
            SetFrontFace(FrontFaceOrder::CCW);
        }
        // Default static model draw
        else 
        {
            if (mesh.vao == 0 || mesh.indexCount == 0) 
            {
                std::cerr << "Invalid mesh for entity " << entity << "\n";
                continue;
            }

            DrawElements(
                DrawMode::Triangles,
                mesh.indexCount,
                DataType::UnsignedInt,
                nullptr
            );
        }

        UnbindVertexArray();
        UnbindProgram();
    }
}
