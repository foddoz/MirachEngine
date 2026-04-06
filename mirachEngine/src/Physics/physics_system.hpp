// === File: physics_system.hpp ===
#pragma once

#include "physics.hpp"
#include "../Transform/transform.hpp"
#include "../ECS/entity_manager.hpp"
#include "../Graphics/Meshes/meshTerrain.hpp"
#include "../Math/math.hpp"
#include <unordered_map>
#include <vector>
#include <iostream>

/**
 * @class PhysicsSystem
 * @brief Manages physics simulation, including rigid body updates and collider handling, in the ECS framework.
 */
class PhysicsSystem 
{
public:

    // === Bitmask categories ===
    static constexpr uint16_t PLAYER_CATEGORY = 0x0001;
    static constexpr uint16_t OBJECT_CATEGORY = 0x0002;
    static constexpr uint16_t TERRAIN_CATEGORY = 0x0004;
    static constexpr uint16_t ROBOT_CATEGORY = 0x0010;

    /**
     * @brief Constructs the PhysicsSystem with a pointer to the EntityManager and initializes the physics world.
     * @param manager Pointer to the ECS EntityManager used to access entity components.
     */
    PhysicsSystem(EntityManager* manager);

    void Reset(EntityManager* newManager);


    /**
     * @brief Destroys and cleans up physics-related components (rigid bodies and colliders) of a given entity.
     * @param e The entity whose physics components should be destroyed.
     */
    void DestroyEntity(Entity e);

    /**
     * @brief Updates the physics world and synchronizes entity transforms with their rigid bodies.
     * @param deltaTime Time step in seconds since the last update.
     */
    void Update(float deltaTime);

    /**
     * @brief Gets the underlying physics world.
     * @return Pointer to the Physics::World object.
     */
    Physics::World* GetWorld() const;

    /**
     * @brief Gets the associated EntityManager pointer.
     * @return Pointer to the ECS EntityManager.
     */
    EntityManager* GetEntityManagerPtr() const;

private:
    EntityManager* m_entityManager;                 ///< Pointer to the ECS EntityManager
    std::unique_ptr<Physics::World> m_world;       ///< Unique pointer to the physics world
};
