#ifndef PLAYER_SCRIPT_HPP
#define PLAYER_SCRIPT_HPP

#include "../ECS/entity_manager.hpp"
#include "../Physics/physics_system.hpp"
#include "../Events/input_manager.hpp"
#include "../Physics/physics.hpp"
#include "../Graphics/Rendering/image_renderer.hpp"
#include "../Camera/camera.hpp"
#include <iostream>

/**
 * @class PlayerScript
 * @brief Manages player-specific input, movement, jumping, and object interaction.
 *
 * Handles input polling, movement physics, grounded checks, object pickup/throwing,
 * and post-physics updates such as held object tracking.
 */
class PlayerScript
{
public:
    /**
     * @brief Constructs the PlayerScript with a reference to the player entity.
     * @param player The player entity ID.
     * @param entityManager ECS manager for component access.
     * @param physicsSystem Reference to the active physics system.
     */
    PlayerScript(const Entity& player, EntityManager* entityManager, PhysicsSystem* physicsSystem);

    /**
     * @brief Captures input states and updates internal flags.
     * @param inputManager Reference to the engine's input system.
     * @return True if the quit signal was received (e.g., ESC pressed), otherwise false.
     */
    bool CaptureInput(InputManager& inputManager);

    /**
     * @brief Applies movement, jumping, and object interaction logic after the physics update.
     * @param deltaTime Time elapsed since last update.
     */
    virtual void PostPhysicsUpdate(float deltaTime) = 0;

    /**
     * @brief Is left mouse button pressed?
     * @return true if pressed
     */
    bool IsLMBPressed() const;

protected:
    /// ID of the player entity this script is controlling.
    Entity m_playerID;

    EntityManager* m_entityManager;
    PhysicsSystem* m_physicsSystem;

    // --- Input State Flags ---
    bool m_moveForward = false;
    bool m_moveBackward = false;
    bool m_moveLeft = false;
    bool m_moveRight = false;
    bool m_jumpPressed = false;
    bool m_spaceDown = false;
    bool m_ctrlDown = false;
    bool m_rightMouseDown = false;
    bool m_leftMousePressed = false;
    bool m_eKeyHeld = false;
    bool m_eKeyPressed = false;
    bool m_exitPressed = false;

    bool m_keyDownR = false;
    bool m_keyDownT = false;
    bool m_keyDownF = false;
    bool m_keyDownC = false;
};

#endif // !PLAYER_SCRIPT_HPP
