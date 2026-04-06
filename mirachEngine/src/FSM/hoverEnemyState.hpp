#pragma once
#include "IEnemyStateLogic.h"
#include "enemy.h"
#include "CommonStates.h"
#include <iostream>
#include "../Graphics/Animation/MD2.hpp"

/**
 * @class hoverEnemyState
 * @brief Defines the state logic for a hovering enemy, including behavior for attack, chase, flee, patrol, death, and pain.
 *
 * This class implements enemy AI transitions and logic through various FSM states using physics and animation control.
 */
class HoverEnemyState : public IEnemyStateLogic 
{
public:

    /**
     * @brief Called when entering the attack state. Plays the attack animation and registers the damage callback.
     * @param e Pointer to the enemy instance.
     */
    void OnAttackEnter(Enemy* e) override;

    /**
     * @brief Handles attacking behavior. Enemy applies force towards the player and rotates to face them.
     * @param e Pointer to the enemy instance.
     */
    void OnAttackExecute(Enemy* e) override;

    /**
     * @brief Called on exiting the attack state. Resets timer and removes damage callback.
     * @param e Pointer to the enemy instance.
     */
    void OnAttackExit(Enemy* e) override;

    /**
     * @brief Called when entering the flee state. Plays the pain animation.
     * @param e Pointer to the enemy instance.
     */
    void OnFleeEnter(Enemy* e) override;

    /**
     * @brief Applies force away from the player and vertically. Transitions back to chase after a delay.
     * @param e Pointer to the enemy instance.
     */
    void OnFleeExecute(Enemy* e) override;

    /**
     * @brief Called on exiting the flee state. Resets internal timer.
     * @param e Pointer to the enemy instance.
     */
    void OnFleeExit(Enemy* e) override;

    /**
     * @brief Called when entering the patrol state. Starts idle animation.
     * @param e Pointer to the enemy instance.
     */
    void OnPatrolEnter(Enemy* e) override;

    /**
     * @brief Enemy moves in a random direction with vertical hovering. Transitions to chase if the player is seen.
     * @param e Pointer to the enemy instance.
     */
    void OnPatrolExecute(Enemy* e) override;

    /**
     * @brief Called on exiting patrol. Resets internal timer.
     * @param e Pointer to the enemy instance.
     */
    void OnPatrolExit(Enemy* e) override;

    /**
     * @brief Called when entering the chase state. Plays the takeoff animation.
     * @param e Pointer to the enemy instance.
     */
    void OnChaseEnter(Enemy* e) override;

    /**
     * @brief Enemy accelerates toward the player and rotates to face them. Switches to attack or returns to patrol based on distance.
     * @param e Pointer to the enemy instance.
     */
    void OnChaseExecute(Enemy* e) override;

    /**
     * @brief Called on exiting the chase state. Resets internal timer.
     * @param e Pointer to the enemy instance.
     */
    void OnChaseExit(Enemy* e) override;

    /**
     * @brief Called when entering the death state. Plays the death animation.
     * @param e Pointer to the enemy instance.
     */
    void OnDeathEnter(Enemy* e) override;

    /**
     * @brief Waits for the death animation to finish before transitioning to patrol.
     * @param e Pointer to the enemy instance.
     */
    void OnDeathExecute(Enemy* e) override;

    /**
     * @brief Marks the enemy as dead by adding the Dead component and resets timer.
     * @param e Pointer to the enemy instance.
     */
    void OnDeathExit(Enemy* e) override;

    /**
     * @brief Called when entering the pain state. Plays the pain animation.
     * @param e Pointer to the enemy instance.
     */
    void OnPainEnter(Enemy* e) override;

    /**
     * @brief Waits for a fixed delay, then transitions to chase.
     * @param e Pointer to the enemy instance.
     */
    void OnPainExecute(Enemy* e) override;

    /**
     * @brief Called on exiting the pain state. Resets internal timer.
     * @param e Pointer to the enemy instance.
     */
    void OnPainExit(Enemy* e) override;
};
