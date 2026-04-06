#pragma once

#include "../physics_system.hpp" // Ensure this includes the CollisionCallback interface
#include "../../ECS/entity_manager.hpp"
#include "../../Graphics/Meshes/meshFile.hpp"
#include "../../Transform/transform.hpp"

#include <unordered_map>
#include <iostream>

/**
 * @class AttachCollisionCallback
 * @brief Handles contact events and interacts with the engine (entities/physics).
 *
 * On contact events, this callback can modify ECS components or issue physics actions
 * via the provided EntityManager and PhysicsSystem.
 */
class AttachCollisionCallback : public Physics::CollisionCallback
{
public:
    /**
     * @brief Construct an attaching callback for a body.
     * @param rb Body to observe (non-owning).
     * @param em Entity manager (non-owning).
     * @param ps Physics system (non-owning).
     */
    AttachCollisionCallback(Physics::RigidBody* rb, EntityManager* em, PhysicsSystem* ps);

    /**
     * @brief Called when contact with another body begins. Stick each body together
     * @param other The other body (non-owning).
     * @param cm Contact manifold for this frame.
     */
    void OnContactEnter(Physics::RigidBody* other, const Physics::ContactManifold& cm) override;

    /**
     * @brief Called while contact with another body continues.
     * @param other The other body (non-owning).
     * @param cm Contact manifold for this frame.
     */
    void OnContactPersist(Physics::RigidBody* other, const Physics::ContactManifold& cm) override;

    /**
     * @brief Called when contact with another body ends.
     * @param other The other body (non-owning).
     */
    void OnContactExit(Physics::RigidBody* other) override;

private:
    EntityManager* m_entityManager; /// ECS entity manager (non-owning)
    PhysicsSystem* m_physicsSystem; /// Physics system interface (non-owning)
};