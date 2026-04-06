#pragma once
#include <string>
#include "StateMachine.h"
#include "../Math/math.hpp"
#include "../Physics/physics_system.hpp"
#include "../ECS/entity_manager.hpp"
#include "CommonStates.h"
#include "../Graphics/Animation/animation_system.hpp"
#include "../Scripts/health.hpp"

class IEnemyStateLogic;

/**
 * @brief Represents an enemy character controlled by a finite state machine (FSM).
 * This class defines properties, behavior, and state transitions for an enemy AI agent.
 * It is integrated with ECS (Entity-Component System), physics, animation, and health systems.
 */
class Enemy 
{
private:
    std::string m_name;  ///< The name/label of the enemy (used for identification or debugging).

    /// Logic interface that implements the behaviors for all states (Attack, Chase, etc.).
    IEnemyStateLogic* m_logic;

    /// Finite State Machine controlling the current behavior state of the enemy.
    StateMachine<Enemy>* m_fsm;

    /// ECS entities used for this enemy and the player (used in AI targeting).
    Entity m_selfEntity;
    Entity* m_playerEntity;
    EntityManager* m_entityManager;

    /// Movement speed of the enemy.
    float m_speed = 5.0f;

    /// Patrol behavior: direction and timing.
    Vector3f m_direction = Vector3f(1.f, 0.f, 0.f);
    float m_timer = 0.f;
    float m_twitchInterval = 3.f;

    /// AI parameters
    const float m_maxChaseDistance = 50.0f; ///< Max distance to chase the player.
    const float m_attackRange = 5.0f;       ///< Distance required to start attacking.
    bool m_isAttacking = false;             ///< Whether currently in attack state.
    float m_damage = 10;                    ///< Damage dealt per attack.

    /// Physics system reference and temporary attack callback used during combat.
    PhysicsSystem* m_physicsSystem = nullptr;
    Physics::CollisionCallback* m_tempAttackCallback = nullptr;

    friend class EnemyState;
    friend class HoverEnemyState;

public:
    /**
     * @brief Default constructor.
     */
    Enemy();

    /**
     * @brief Full constructor for initializing enemy state and dependencies.
     *
     * @param name         Name of the enemy.
     * @param self         This entity's ID.
     * @param player       Pointer to player entity ID.
     * @param mgr          Pointer to the EntityManager.
     * @param AILogic      Pointer to the AI logic implementation.
     * @param physics      Pointer to the physics system.
     */
    Enemy(const std::string& name, Entity self, Entity* player, EntityManager* mgr,
        IEnemyStateLogic* AILogic, PhysicsSystem* physics);

    /**
     * @brief Destructor.
     */
    ~Enemy();

    /**
     * @brief Per-frame update for the enemy. Delegates to the FSM.
     * @param dt Delta time since last frame.
     */
    void Update(float dt);

    /// Getters and Setters

    float GetSpeed() const;
    void SetSpeed(float s);
    EntityManager* GetEntityManager() const;
    Entity GetSelfEntity() const;
    Entity* GetPlayerEntity() const;
    std::string GetName() const;
    StateMachine<Enemy>* GetFSM() const;
    float GetDamage() const;

    /**
     * @brief Rotates the enemy to face a specific target position.
     * @param targetPos Target world-space position to look at.
     */
    void FaceTarget(const Vector3f& targetPos);

    /**
     * @brief Sets the current animation by name.
     * @param animName Name of the animation (e.g., "walk", "att_c").
     */
    void SetAnimation(const std::string& animName);

    /**
     * @brief Checks whether the player is within a given range.
     * @param e     Pointer to the enemy (used to query position).
     * @param range Max detection range.
     * @return true if player is within range.
     */
    bool IsPlayerInRange(Enemy* e, float range);

    /**
     * @brief Returns whether the current animation has ended.
     */
    bool IsAnimationEnded() const;

    /**
     * @brief Checks if the enemy's health is below a threshold.
     * @param threshold A normalized value.
     */
    bool IsHealthLow(float threshold) const;

    /**
     * @brief Notifies nearby enemies (if implemented) that the player is detected.
     */
    void AlertNearbyEnemies();

    /// Physics Accessors

    PhysicsSystem* GetPhysicsSystem() const;
    void SetAttackCallback(Physics::CollisionCallback* cb);
    void ClearAttackCallback();

    /// === FSM State Hooks ===

    void OnAttackEnter();
    void OnAttackExecute();
    void OnAttackExit();

    void OnFleeEnter();
    void OnFleeExecute();
    void OnFleeExit();

    void OnPatrolEnter();
    void OnPatrolExecute();
    void OnPatrolExit();

    void OnChaseEnter();
    void OnChaseExecute();
    void OnChaseExit();

    void OnDeathEnter();
    void OnDeathExecute();
    void OnDeathExit();

    void OnPainEnter();
    void OnPainExecute();
    void OnPainExit();
};
