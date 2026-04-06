#pragma once

#include "../../ECS/entity_manager.hpp"
#include "MD2.hpp"
#include "../Rendering/mesh_renderer.hpp"

/**
 * @struct MD2Animator
 * @brief ECS component for animating an entity with an MD2 mesh.
 *
 * Stores the current animation state for an entity and performs per-entity frame interpolation.
 * Each entity shares a reference to a read-only MD2 mesh and owns its own animated vertex buffer.
 */
struct MD2Animator 
{
    MeshMD2* mesh = nullptr;                         ///< Pointer to shared, immutable MD2 mesh.

    std::string currentAnim = "idle";                ///< Name of the animation currently playing.
    std::string lastAppliedAnim = "";                ///< Last animation applied, used to detect animation changes.

    float animTime = 0.0f;                           ///< Accumulated time for frame interpolation.
    int currentFrame = 0;                            ///< Current frame index in the animation.
    int nextFrame = 1;                               ///< Next frame index in the animation.

    unsigned int animatedVBO = 0;                    ///< GPU buffer storing interpolated vertices for this entity.
    std::vector<Vector3f> interpolatedVerts;         ///< CPU-side buffer storing interpolated vertex data.
};

/**
 * @class AnimationSystem
 * @brief System responsible for updating MD2 animations for all entities with MD2Animator components.
 *
 * Performs time-based animation updates, interpolates vertex positions,
 * and uploads results to entity-specific vertex buffers for rendering.
 */
class AnimationSystem 
{
public:
    /**
     * @brief Updates all MD2Animator components in the ECS based on elapsed time.
     * @param em Reference to the EntityManager.
     * @param deltaTime Time elapsed since the last frame (in seconds).
     */
    void Update(EntityManager& em, float deltaTime);
};
