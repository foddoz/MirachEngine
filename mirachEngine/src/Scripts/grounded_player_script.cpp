#include "grounded_player_script.hpp"

#include "../Transform/transform.hpp"
#include "../Physics/CollisionCallbacks/log_collision_callback.hpp"

GroundedPlayerScript::GroundedPlayerScript(const Entity& player, EntityManager* entityManager, PhysicsSystem* physicsSystem)
	: 
    PlayerScript(player, entityManager, physicsSystem), 
    m_canDoubleJump(false),
    m_userTilt(Quaternion(1, 0, 0, 0)),
    m_initialTilt(Quaternion(1, 0, 0, 0))
{
}

void GroundedPlayerScript::PostPhysicsUpdate(float deltaTime)
{
    const float moveSpeed = 1.0f;

    // Ignore if player has no rigidbody
    if (!m_entityManager->HasComponent<Physics::RigidBody*>(m_playerID))
    {
        return;
    }

    float vertical = 0.f;
    float horizontal = 0.f;

    // Determine movement direction
    if (m_moveForward)
    {
        vertical += moveSpeed;
    }
    if (m_moveBackward)
    {
        vertical -= moveSpeed;
    }
    if (m_moveLeft)
    {
        horizontal -= moveSpeed;
    }
    if (m_moveRight)
    {
        horizontal += moveSpeed;
    }

    // Handle jumping or movement if grounded
    if (IsGrounded()) 
    {
        if (m_jumpPressed)
        {
            Jump();
        }
        ApplyPlayerMovement(vertical, horizontal);
    }
    else if (m_canDoubleJump && m_jumpPressed) 
    {
        // Double jump allowed only once while airborne
        DoubleJump(vertical, horizontal);
        m_canDoubleJump = false;
    }

    if (m_eKeyPressed && m_heldEntity == INVALID_ENTITY)
    {
        PickupObject();
    }
    else if (m_eKeyPressed)
    {
        DropObject();
    }
    else
    {
        UpdateHeldObject();
    }

    float pitch = 0.f;
    float roll = 0.f;

    if (m_keyDownR)
    {
        roll -= 1.f;
    }
    if (m_keyDownT)
    {
        roll += 1.f;
    }
    if (m_keyDownF)
    {
        pitch -= 1.f;
    }
    if (m_keyDownC)
    {
        pitch += 1.f;
    }

    RotateHeldObject(roll, pitch, deltaTime);

    // Launch object on left click
    if (m_leftMousePressed) 
    {
        LaunchObject();
    }

    // Cooldown logic for re-interacting with recently thrown objects
    if (m_throwCooldownTimer > 0.0f)
    {
        m_throwCooldownTimer -= deltaTime;
    }

    if (m_throwCooldownTimer <= 0.0f && m_lastThrownEntity != INVALID_ENTITY && m_entityManager->HasComponent<Physics::RigidBody*>(m_lastThrownEntity))
    {
        m_lastThrownEntity = INVALID_ENTITY;
    }
}

void GroundedPlayerScript::ApplyPlayerMovement(float oscillateSpeed, float horizontalSpeed)
{
    Physics::RigidBody* rb = m_entityManager->GetComponent<Physics::RigidBody*>(m_playerID);
    Camera& cam = m_entityManager->GetComponent<Camera>(m_playerID);

    // Combine front and right vectors based on input
    Vector3f moveDirection = cam.GetFrontVector() * oscillateSpeed + cam.GetRightVector() * horizontalSpeed;

    if (Length(moveDirection) > 0.0f)
    {
        moveDirection = Normalize(moveDirection);

        const float moveSpeed = 75.f;

        // Apply horizontal impulse (no vertical impulse for grounded move)
        Vector3f impulse = moveDirection * moveSpeed;

        Vector3f initialVelocity = rb->GetLinearVelocity();
        Vector2f horizontalVelocity = { initialVelocity.x, initialVelocity.z };
        if (Length(horizontalVelocity) < 10.f)
        {
            rb->ApplyLinearForce(Vector3f(impulse.x, 0.f, impulse.z));
        }
    }
}

void GroundedPlayerScript::Jump()
{
    Physics::RigidBody* rb = m_entityManager->GetComponent<Physics::RigidBody*>(m_playerID);

    // Apply upward force for jump
    //rb->ApplyLinearImpulse({ 0.0f, 15.0f, 0.0f });
    rb->ApplyLinearForce({ 0.0f, 1500.0f, 0.0f });
}


void GroundedPlayerScript::DoubleJump(float oscillateSpeed, float horizontalSpeed)
{
    Physics::RigidBody* rb = m_entityManager->GetComponent<Physics::RigidBody*>(m_playerID);
    Camera& cam = m_entityManager->GetComponent<Camera>(m_playerID);

    // Build movement direction
    Vector3f moveDirection = cam.GetFrontVector() * oscillateSpeed + cam.GetRightVector() * horizontalSpeed;

    if (Length(moveDirection) > 0.0f && rb->IsDynamic())
    {
        moveDirection = Normalize(moveDirection);
        Vector3f currentVelocity = rb->GetLinearVelocity();
        const float moveSpeed = 20.0f;

        // Adjust velocity while preserving vertical speed
        float currentMag = Length(Vector2f(currentVelocity.x, currentVelocity.z));
        Vector3f targetVelocity = moveDirection * std::max(currentMag, moveSpeed);
        rb->SetLinearVelocity({ targetVelocity.x, currentVelocity.y, targetVelocity.z });
    }
}


bool GroundedPlayerScript::IsGrounded()
{
    Vector3f origin = m_entityManager->GetComponent<Transform>(m_playerID).GetPosition() - Vector3f(0.f, 2.5f, 0.f);
    Vector3f direction = Vector3f(0.f, -1.f, 0.f);

    float maxDistance = 2.0f;
    Physics::Ray ray(origin, direction);
    Physics::RaycastHit result = m_physicsSystem->GetWorld()->Raycast(ray, maxDistance);

    return result.hit;
}


void GroundedPlayerScript::LaunchObject()
{
    Camera cam = m_entityManager->GetComponent<Camera>(m_playerID);
    Vector3f origin = m_entityManager->GetComponent<Transform>(m_playerID).GetPosition();
    Vector3f direction = Normalize(cam.GetFrontVector());
    Vector3f force = direction * 30.f;

    if (m_heldEntity != INVALID_ENTITY) 
    {
        // Launch currently held object
        Physics::RigidBody* rb = m_entityManager->GetComponent<Physics::RigidBody*>(m_heldEntity);
        LaunchObjectInternal(m_heldEntity, rb, force);
        DropObject();
    }
    else 
    {
        // Raycast to detect object in view
        float maxDistance = 8.0f;
        Physics::Ray ray(origin, direction);
        Physics::RaycastHit result = m_physicsSystem->GetWorld()->Raycast(ray, maxDistance);

        if (result.hit && result.body) 
        {
            // Find ECS entity matching the body
            for (const auto& [entity, rb] : m_entityManager->GetAllComponents<Physics::RigidBody*>())
            {
                if (rb == result.body && GetValidRigidBody(entity))
                {
                    LaunchObjectInternal(entity, rb, force);
                    break;
                }
            }
        }
    }
}

void GroundedPlayerScript::LaunchObjectInternal(Entity object, Physics::RigidBody* rb, Vector3f force)
{
    rb->ApplyLinearImpulse(force);
    m_lastThrownEntity = object;

    m_throwCooldownTimer = m_throwCooldownDuration;
}

void GroundedPlayerScript::PickupObject()
{
    Physics::RaycastHit result = RaycastFromLineOfSight(100.f);

    if (result.hit) 
    {
        // You can access result.collider to find the associated entity
        Physics::Collider* hitCollider = static_cast<Physics::Collider*>(result.collider);

        if (hitCollider && result.body && result.body->IsDynamic()) 
        {
            for (const auto& [entity, rb] : m_entityManager->GetAllComponents<Physics::RigidBody*>()) 
            {
                if (GetValidRigidBody(entity) == result.body) 
                {
                    rb->SetAngularVelocity(Vector3f(0.f));
                    rb->SetAngularFreeAxis(Vector3f(1.f));
                    m_heldEntity = entity;

                    Camera& cam = m_entityManager->GetComponent<Camera>(m_playerID);
                    m_initialTilt = RemoveYaw(rb->GetOrientation());
                    m_userTilt = Quaternion(1, 0, 0, 0);
                    break;
                }
            }
        }
    }
}

void GroundedPlayerScript::DestroyDynamicObject()
{
    Physics::RaycastHit result = RaycastFromLineOfSight(100.f);

    if (result.hit)
    {
        if (result.body && result.body->IsDynamic())
        {
            for (const auto& [entity, rb] : m_entityManager->GetAllComponents<Physics::RigidBody*>())
            {
                if (GetValidRigidBody(entity) == result.body && entity != m_lastThrownEntity)
                {
                    m_physicsSystem->DestroyEntity(entity);
                    m_entityManager->DestroyEntity(entity);
                    m_heldEntity = INVALID_ENTITY;
                    break;
                }
            }
        }
    }
}

void GroundedPlayerScript::RotateHeldObject(float roll, float pitch, float dt)
{
    if (m_heldEntity != INVALID_ENTITY)
    {
        const float rollSpeed = ToRadians(90.f);  // 90?s
        const float pitchSpeed = ToRadians(90.f);

        float dRoll = roll * rollSpeed * dt;
        float dPitch = pitch * pitchSpeed * dt;

        // Apply pitch about LOCAL X and roll about LOCAL Z of the current tilt
        Quaternion qPitch = AngleAxis(dPitch, Vector3f(1, 0, 0));
        Quaternion qRoll = AngleAxis(dRoll, Vector3f(0, 0, 1));

        // Order matters: first pitch, then roll
        m_userTilt = Normalize(qRoll * qPitch * m_userTilt);
    }
}

Physics::RaycastHit GroundedPlayerScript::RaycastFromLineOfSight(float maxDistance)
{
    Camera& cam = m_entityManager->GetComponent<Camera>(m_playerID);
    Vector3f origin = m_entityManager->GetComponent<Transform>(m_playerID).GetPosition() + cam.GetLocalPosition();
    Vector3f dir = Normalize(cam.GetFrontVector());

    Physics::Ray ray(origin, dir);
    Physics::RaycastHit result = m_physicsSystem->GetWorld()->Raycast(ray, maxDistance);

    return result;
}

void GroundedPlayerScript::UpdateHeldObject()
{
    if (m_heldEntity == INVALID_ENTITY)
    {
        return;
    }

    Physics::RigidBody* rb = m_entityManager->GetComponent<Physics::RigidBody*>(m_heldEntity);
    Camera& cam = m_entityManager->GetComponent<Camera>(m_playerID);
    Transform& tf = m_entityManager->GetComponent<Transform>(m_playerID);

    // === Position follow === //
    Vector3f target = tf.GetPosition() + cam.GetLocalPosition() + cam.GetFrontVector() * m_holdDistance;
    target.y -= 1.5f;

    Vector3f direction = target - rb->GetPosition();
    rb->SetLinearVelocity(direction * 10.0f);

    // === Yaw-only orientation follow === //
    Quaternion yawNow = MakeYawFromCamera(cam);
    Quaternion desired = Normalize(yawNow * m_userTilt * m_initialTilt);

    rb->SetOrientation(desired);
    rb->SetAngularVelocity(Vector3f(0.f));
}

void GroundedPlayerScript::DropObject()
{
    if (m_heldEntity != INVALID_ENTITY) 
    {
        Physics::RigidBody* rb = m_entityManager->GetComponent<Physics::RigidBody*>(m_heldEntity);
        rb->SetAngularFreeAxis(Vector3f(1.f)); // Re-enable full rotation

        //std::cout << "Dropping object\n";
        m_heldEntity = INVALID_ENTITY;
    }
}

Quaternion GroundedPlayerScript::MakeYawFromCamera(const Camera& cam)
{
    Vector3f camF = cam.GetFrontVector();
    Vector3f yawDir = Vector3f(camF.x, 0.f, camF.z);

    Vector3f f = Normalize(yawDir); //Safe to normalize unless camera pitch can be at 90.f or -90.f
    Vector3f x = Normalize(Cross(Vector3f(0.f, 1.f, 0.f), f));
    Vector3f y = Cross(f, x);

    Matrix3f R(1.f);
    R[0] = x;
    R[1] = y;
    R[2] = f;

    return ToQuaternion(R);
}

Physics::RigidBody* GroundedPlayerScript::GetValidRigidBody(Entity object)
{
    if (object == INVALID_ENTITY || !m_entityManager->HasComponent<Physics::RigidBody*>(object))
    {
        return nullptr;
    }
    auto* rb = m_entityManager->GetComponent<Physics::RigidBody*>(object);
    return (rb) ? rb : nullptr;
}

Physics::Collider* GroundedPlayerScript::GetValidCollider(Entity object)
{
    if (object == INVALID_ENTITY || !m_entityManager->HasComponent<Physics::Collider*>(object))
    {
        return nullptr;
    }
    auto* collider = m_entityManager->GetComponent<Physics::Collider*>(object);

    try 
    {
        if (collider)
        {
            return collider;
        }
    }
    catch (...) 
    {
        std::cerr << "[GetValidCollider] Caught exception ?collider may be deleted\n";
    }

    return nullptr;
}

void GroundedPlayerScript::SetImageRenderer(ImageRenderer* renderer, Texture* image)
{
    m_imageRenderer = renderer;
    m_exitTexture = image;
}

void GroundedPlayerScript::DrawUI(float mScreenWidth, float mScreenHeight)
{
    if (m_exitPressed && m_imageRenderer && m_exitTexture) 
    {
        glViewport(0, 0, mScreenWidth, mScreenHeight);
        glDisable(GL_DEPTH_TEST);
        glDepthMask(GL_FALSE);
        glDisable(GL_CULL_FACE);

        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        m_imageRenderer->Draw(*m_exitTexture, 0.0f, 0.0f, 1920, 1080);
        glDepthMask(GL_TRUE);
        glEnable(GL_DEPTH_TEST);
        glEnable(GL_CULL_FACE);
    }
}

bool GroundedPlayerScript::EndScreen(float dt, float mScreenWidth, float mScreenHeight) 
{
    if (m_exitPressed && m_imageRenderer && m_exitTexture && !endGame) 
    {
        endGame = true;
        dt = 0.0f;
    }
    if (endGame) 
    {
        GroundedPlayerScript::DrawUI(mScreenWidth, mScreenHeight);
        timer += dt;
        if (timer >= 5.0f) 
        {
            return true;
        }
        return false;
    }
    return false;
}