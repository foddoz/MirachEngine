#include "player_script.hpp"

#ifndef GROUNDED_PLAYER_SCRIPT_HPP
#define GROUNDED_PLAYER_SCRIPT_HPP

/**
 * @class PlayerScript
 * @brief Manages player-specific input, movement, jumping, and object interaction.
 *
 * Handles input polling, movement physics, grounded checks, object pickup/throwing,
 * and post-physics updates such as held object tracking.
 */
class GroundedPlayerScript : public PlayerScript
{
public:
    /**
     * @brief Constructs the PlayerScript with a reference to the player entity. Moves and jump on ground.
     * @param player The player entity ID.
     */
    GroundedPlayerScript(const Entity& player, EntityManager* entityManager, PhysicsSystem* physicsSystem);

    /**
     * @brief Applies movement, jumping, and object interaction logic after the physics update.
     * @param entityManager ECS manager for component access.
     * @param physicsSystem Reference to the active physics system.
     * @param deltaTime Time elapsed since last update.
     */
    void PostPhysicsUpdate(float deltaTime) override;

    /// Setting Image
    void SetImageRenderer(ImageRenderer* renderer, Texture* image);

    /// Called Image
    void DrawUI(float mScreenWidth, float mScreenHeight);

    bool EndScreen(float dt, float mScreenWidth, float mScreenHeight);

private:
    /**
     * @brief Applies directional player movement based on input.
     * @param em EntityManager reference.
     * @param oscillateSpeed Speed factor for movement oscillation (used in air).
     * @param horizontalSpeed Base horizontal movement speed.
     */
    void ApplyPlayerMovement(float oscillateSpeed, float horizontalSpeed);

    /**
     * @brief Applies vertical force for a normal jump if grounded.
     * @param em EntityManager reference.
     */
    void Jump();

    /**
     * @brief Performs a double jump if eligible.
     * @param em EntityManager reference.
     * @param oscillateSpeed Speed factor for movement oscillation.
     * @param horizontalSpeed Base movement speed.
     */
    void DoubleJump(float oscillateSpeed, float horizontalSpeed);

    /**
     * @brief Checks whether the player is currently grounded.
     * @param em EntityManager reference.
     * @param ps Physics system reference.
     * @return True if grounded, false otherwise.
     */
    bool IsGrounded();

    /**
     * @brief Attempts to launch a held object forward with force.
     * @param em EntityManager reference.
     * @param ps Physics system reference.
     */
    void LaunchObject();

    /**
     * @brief Applies an internal impulse to a specific object.
     * @param em EntityManager reference.
     * @param ps Physics system reference.
     * @param object The target entity to launch.
     * @param rb Pointer to the rigid body of the object.
     * @param force Force vector to apply.
     */
    void LaunchObjectInternal(Entity object, Physics::RigidBody* rb, Vector3f force);

    /**
     * @brief Performs object pickup logic using raycasting.
     * @param em EntityManager reference.
     * @param ps Physics system reference.
     */
    void PickupObject();

    /**
     * @brief Destroy a dynamic object that the player is looking at.
     */
    void DestroyDynamicObject();

    //void IdentifyBoxState(EntityManager& em, PhysicsSystem& ps);

    /**
     * @brief Rotates the currently held object based on user roll and pitch input.
     * @param roll  Roll input value (e.g., from controller/keyboard).
     * @param pitch Pitch input value (e.g., from controller/keyboard).
     * @param dt    Time step in seconds.
     */
    void RotateHeldObject(float roll, float pitch, float dt);

    /**
     * @brief Raycast from the player's camera forward direction.
     * @param maxDistance Maximum distance to trace along the camera forward axis.
     * @return Physics::RaycastHit Information about the raycast result (hit flag, body, position, etc.).
     */
    Physics::RaycastHit RaycastFromLineOfSight(float maxDistance);

    /**
     * @brief Continuously updates the held object's position and physics state.
     * @param em EntityManager reference.
     */
    void UpdateHeldObject();

    /**
     * @brief Drops the currently held object.
     * @param em EntityManager reference.
     */
    void DropObject();

    /**
     * @brief Create a yaw-only orientation from the camera's forward direction.
     * @param cam Reference to the camera to take the direction from.
     * @return Quaternion representing the yaw orientation derived from the camera.
     */
    Quaternion MakeYawFromCamera(const Camera& cam);

    /**
     * @brief Gets a valid rigid body pointer from an entity.
     * @param em EntityManager reference.
     * @param object Target entity.
     * @return Pointer to the RigidBody if valid; otherwise nullptr.
     */
    Physics::RigidBody* GetValidRigidBody(Entity object);

    /**
     * @brief Gets a valid collider pointer from an entity.
     * @param em EntityManager reference.
     * @param object Target entity.
     * @return Pointer to the Collider if valid; otherwise nullptr.
     */
    Physics::Collider* GetValidCollider(Entity object);

    /// Whether the player can perform a double jump.
    bool m_canDoubleJump;

    /// The currently held object (if any).
    Entity m_heldEntity = INVALID_ENTITY;

    Quaternion m_initialTilt;
    Quaternion m_userTilt;

    /// Distance from player to hold picked-up objects.
    float m_holdDistance = 4.0f;

    /// Last thrown object to avoid re-pickup.
    Entity m_lastThrownEntity = INVALID_ENTITY;

    /// Time remaining before another object can be thrown.
    float m_throwCooldownTimer = 0.0f;

    /// Cooldown duration between throws.
    float m_throwCooldownDuration = 3.5f;

    //--- Exit Image ---
    ImageRenderer* m_imageRenderer = nullptr;
    Texture* m_exitTexture = nullptr;

    //--- Timer ---
    float timer = 0.0f;
    bool endGame = false;
};

#endif