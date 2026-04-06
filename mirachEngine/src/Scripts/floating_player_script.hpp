#include "player_script.hpp"

#ifndef FLOATING_PLAYER_SCRIPT_HPP
#define FLOATING_PLAYER_SCRIPT_HPP

/*
 * @class PlayerScript
 * @brief Manages player-specific input, movement, jumping, and object interaction.
 *
 * Handles input polling, movement physics, grounded checks, object pickup/throwing,
 * and post-physics updates such as held object tracking.
 */
class FloatingPlayerScript : public PlayerScript
{
public:
    /**
     * @brief Constructs the PlayerScript with a reference to the player entity. Floats around.
     * @param player The player entity ID.
     */
    FloatingPlayerScript(const Entity& player, EntityManager* entityManager, PhysicsSystem* physicsSystem);

    /**
     * @brief Applies horizontal and vertical movement.
     * @param entityManager ECS manager for component access.
     * @param physicsSystem Reference to the active physics system.
     * @param deltaTime Time elapsed since last update.
     */
    void PostPhysicsUpdate(float deltaTime) override;

private:
    /**
     * @brief Applies directional player movement based on input.
     * @param em EntityManager reference.
     * @param oscillateSpeed Speed factor for movement oscillation (used in air).
     * @param horizontalSpeed Base horizontal movement speed.
     */
    void ApplyPlayerMovement(float oscillateSpeed, float horizontalSpeed, float deltaTime);

    void ApplyVerticalMovement(float verticalSpeed, float deltaTime);
};

#endif // !PLAYER_SCRIPT_HPP
