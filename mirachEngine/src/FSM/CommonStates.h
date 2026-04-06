#ifndef COMMON_STATES_H
#define COMMON_STATES_H

#include <iostream>

#include "state.h"
#include "singleton.h"


/**
 * @brief Represents the Attack state for a finite state machine.
 * Calls OnAttackEnter, OnAttackExecute, and OnAttackExit on the entity.
 * @tparam Entity The type of entity using this state.
 */
template <typename Entity>
class Attack : public State<Entity> 
{
public:
    void Enter(Entity* e) override 
    {
        e->OnAttackEnter();
    }

    void Execute(Entity* e) override 
    {
        e->OnAttackExecute();
    }

    void Exit(Entity* e) override 
    {
        e->OnAttackExit();
    }
};

/**
 * @brief Represents the Flee state.
 * Calls OnFleeEnter, OnFleeExecute, and OnFleeExit on the entity.
 */
template <typename Entity>
class Flee : public State<Entity> 
{
public:
    void Enter(Entity* e) override 
    {
        e->OnFleeEnter();
    }

    void Execute(Entity* e) override 
    {
        e->OnFleeExecute();
    }

    void Exit(Entity* e) override 
    {
        e->OnFleeExit();
    }
};

/**
 * @brief Represents the Patrol state.
 * Calls OnPatrolEnter, OnPatrolExecute, and OnPatrolExit on the entity.
 */
template <typename Entity>
class Patrol : public State<Entity> 
{
public:
    void Enter(Entity* e) override 
    {
        e->OnPatrolEnter();
    }

    void Execute(Entity* e) override 
    {
        e->OnPatrolExecute();
    }

    void Exit(Entity* e) override 
    {
        e->OnPatrolExit();
    }
};

/**
 * @brief Represents the Chase state.
 * Calls OnChaseEnter, OnChaseExecute, and OnChaseExit on the entity.
 */
template <typename Entity>
class Chase : public State<Entity> 
{
public:
    void Enter(Entity* e) override 
    {
        e->OnChaseEnter();
    }

    void Execute(Entity* e) override 
    {
        e->OnChaseExecute();
    }

    void Exit(Entity* e) override 
    {
        e->OnChaseExit();
    }
};

/**
 * @brief Represents the Death state.

 * Calls OnDeathEnter, OnDeathExecute, and OnDeathExit on the entity.
 */
template <typename Entity>
class Death : public State<Entity> 
{
public:
    void Enter(Entity* e) override 
    {
        e->OnDeathEnter();
    }

    void Execute(Entity* e) override 
    {
        e->OnDeathExecute();
    }

    void Exit(Entity* e) override 
    {
        e->OnDeathExit();
    }
};

/**
 * @brief Represents the Pain (e.g., hit reaction) state.
 * Calls OnPainEnter, OnPainExecute, and OnPainExit on the entity.
 */
template <typename Entity>
class Pain : public State<Entity> 
{
public:
    void Enter(Entity* e) override 
    {
        e->OnPainEnter();
    }

    void Execute(Entity* e) override 
    {
        e->OnPainExecute();
    }

    void Exit(Entity* e) override 
    {
        e->OnPainExit();
    }
};

// ----------------------------------------------------------------------
// Singleton Aliases for Shared State Instances
// ----------------------------------------------------------------------

/**
 * @brief Singleton instance alias for Attack state.
 */
template <typename Entity>
using AttackState = Singleton<Attack<Entity>>;

/**
 * @brief Singleton instance alias for Flee state.
 */
template <typename Entity>
using FleeState = Singleton<Flee<Entity>>;

/**
 * @brief Singleton instance alias for Patrol state.
 */
template <typename Entity>
using PatrolState = Singleton<Patrol<Entity>>;

/**
 * @brief Singleton instance alias for Chase state.
 */
template <typename Entity>
using ChaseState = Singleton<Chase<Entity>>;

/**
 * @brief Singleton instance alias for Death state.
 */
template <typename Entity>
using DeathState = Singleton<Death<Entity>>;

/**
 * @brief Singleton instance alias for Pain state.
 */
template <typename Entity>
using PainState = Singleton<Pain<Entity>>;

#endif // COMMON_STATES_H
