#pragma once

#include "../physics.hpp" // Ensure this includes the CollisionCallback interface

#include <unordered_map>
#include <iostream>
#include <set>

/**
 * @class LogCollisionCallback
 * @brief Logs contact events (enter/persist/exit) for a specific body.
 */
class LogCollisionCallback : public Physics::CollisionCallback
{
public:
    /**
     * @brief Construct a logging callback for a body.
     * @param rb Body to observe (non-owning).
     */
    LogCollisionCallback(Physics::RigidBody* rb);

    /**
     * @brief Called when contact with another body begins. Logs information about the contact to the console.
     * @param rb The other body (non-owning).
     * @param cm Contact manifold for this frame.
     */
    void OnContactEnter(Physics::RigidBody* rb, const Physics::ContactManifold& cm) override;

    /**
     * @brief Called while contact with another body continues.
     * @param rb The other body (non-owning).
     * @param cm Contact manifold for this frame.
     */
    void OnContactPersist(Physics::RigidBody* rb, const Physics::ContactManifold& cm) override;

    /**
     * @brief Called when contact with another body ends.
     * @param rb The other body (non-owning).
     */
    void OnContactExit(Physics::RigidBody* rb) override;
};