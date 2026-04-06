#pragma once

//#include "../Physics/physics.hpp"
#include "enemy.h"
#include "../Physics/CollisionCallbacks/log_collision_callback.hpp"
//#include "enemy.h"

// class Enemy;

/**
 * @brief Interface class defining all enemy state transition logic.
 * This interface is designed to separate the behavior of different enemy states
 * (Attack, Flee, Patrol, Chase, Death, Pain) into clearly defined `Enter`, `Execute`, and `Exit` methods.
 * Implementing classes should define these methods for each state.
 */
class IEnemyStateLogic 
{
public:
    /**
     * @brief Virtual destructor to allow proper cleanup of derived classes.
     */
    virtual ~IEnemyStateLogic() = default;

    // === Attack State ===

    /**
     * @brief Called when entering the attack state.
     * @param e Pointer to the Enemy entity.
     */
    virtual void OnAttackEnter(Enemy* e) = 0;

    /**
     * @brief Called every frame while in the attack state.
     * @param e Pointer to the Enemy entity.
     */
    virtual void OnAttackExecute(Enemy* e) = 0;

    /**
     * @brief Called when exiting the attack state.
     * @param e Pointer to the Enemy entity.
     */
    virtual void OnAttackExit(Enemy* e) = 0;

    // === Flee State ===

    virtual void OnFleeEnter(Enemy* e) = 0;
    virtual void OnFleeExecute(Enemy* e) = 0;
    virtual void OnFleeExit(Enemy* e) = 0;

    // === Patrol State ===

    virtual void OnPatrolEnter(Enemy* e) = 0;
    virtual void OnPatrolExecute(Enemy* e) = 0;
    virtual void OnPatrolExit(Enemy* e) = 0;

    // === Chase State ===

    virtual void OnChaseEnter(Enemy* e) = 0;
    virtual void OnChaseExecute(Enemy* e) = 0;
    virtual void OnChaseExit(Enemy* e) = 0;

    // === Death State ===

    virtual void OnDeathEnter(Enemy* e) = 0;
    virtual void OnDeathExecute(Enemy* e) = 0;
    virtual void OnDeathExit(Enemy* e) = 0;

    // === Pain State ===

    virtual void OnPainEnter(Enemy* e) = 0;
    virtual void OnPainExecute(Enemy* e) = 0;
    virtual void OnPainExit(Enemy* e) = 0;

protected:
    /**
     * @brief Registers a collision callback to apply damage to the player.
     *
     * This should be called when the enemy begins an attack.
     *
     * @param e Pointer to the Enemy performing the attack.
     */
    void RegisterAttackCallback(Enemy* e) 
    {
        if (!e->GetPlayerEntity())
        {
            return;
        }

        Entity player = *e->GetPlayerEntity();
        Entity self = e->GetSelfEntity();
        /*
        Physics::CollisionCallback* callback = new EnemyAttackCallback(e->GetEntityManager(), self, player);
        e->GetPhysicsSystem()->GetWorld()->RegisterCollisionCallback(callback);
        e->SetAttackCallback(callback);
        */
    }

    /**
     * @brief Clears the previously registered attack callback.
     *
     * This should be called when the attack ends or the enemy exits the attack state.
     *
     * @param e Pointer to the Enemy.
     */
    void CleanupAttackCallback(Enemy* e) 
    {
        e->ClearAttackCallback();
    }
};
