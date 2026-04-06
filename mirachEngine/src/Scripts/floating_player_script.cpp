#include "floating_player_script.hpp"

FloatingPlayerScript::FloatingPlayerScript(const Entity& player, EntityManager* entityManager, PhysicsSystem* physicsSystem)
	: 
    PlayerScript(player, entityManager, physicsSystem)
{

}

void FloatingPlayerScript::PostPhysicsUpdate(float deltaTime)
{
    const float moveSpeed = 1.0f;

    float oscillate = 0.f;
    float horizontal = 0.f;
    float vertical = 0.f;

    // Determine movement direction
    if (m_moveForward)
    {
        oscillate += moveSpeed;
    }
    if (m_moveBackward)
    {
        oscillate -= moveSpeed;
    }
    if (m_moveLeft)
    {
        horizontal -= moveSpeed;
    }
    if (m_moveRight)
    {
        horizontal += moveSpeed;
    }

    ApplyPlayerMovement(oscillate, horizontal, deltaTime);

    if (m_spaceDown)
    {
        vertical += moveSpeed;
    }
    if (m_ctrlDown)
    {
        vertical -= moveSpeed;
    }

    if (vertical != 0.f)
    {
        ApplyVerticalMovement(vertical, deltaTime);
    }
}

void FloatingPlayerScript::ApplyPlayerMovement(float oscillateSpeed, float horizontalSpeed, float deltaTime)
{
    Transform& tf = m_entityManager->GetComponent<Transform>(m_playerID);
    Camera& cam = m_entityManager->GetComponent<Camera>(m_playerID);

    // Combine front and right vectors based on input
    Vector3f moveDirection = cam.GetFrontVector() * oscillateSpeed + cam.GetRightVector() * horizontalSpeed;

    const float moveSpeed = 5.f;

    // Apply horizontal impulse (no vertical impulse for grounded move)
    Vector3f velocity = moveDirection * moveSpeed;

    Vector3f initialPos = tf.GetPosition();
    tf.SetPosition(initialPos + velocity * deltaTime);
}

/**
 * @brief Applies vertical movement to the floating player.
 * @param verticalSpeed Raw vertical input speed (will be scaled internally).
 * @param deltaTime     Time step in seconds.
 */
void FloatingPlayerScript::ApplyVerticalMovement(float verticalSpeed, float deltaTime)
{
    Transform& tf = m_entityManager->GetComponent<Transform>(m_playerID);
    verticalSpeed *= 5.f;

    Vector3f initialPos = tf.GetPosition();
    tf.SetPosition(initialPos + Vector3f(0.f, verticalSpeed, 0.f) * deltaTime);
}