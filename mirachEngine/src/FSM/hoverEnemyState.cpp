#include "hoverEnemyState.hpp"
#include "../Scripts/health.hpp"

#include "../Physics/CollisionCallbacks/log_collision_callback.hpp"
#include "../Physics/physics_system.hpp"
/*
            stand: 0 to 29
                forwrd : 30 to 64
                death : 162 to 172
                pain : 113 to 161
                stop : 65 to 81
                takeof : 82 to 111
                land : 112 to 112
                backwd : 173 to 196
                attak : 197 to 204
                */

void HoverEnemyState::OnAttackEnter(Enemy* e)
{
    // Play attack animation and reset timer
    e->SetAnimation("attak");
    e->m_timer = 0.f;

    // Register damage/collision callback during attack
    RegisterAttackCallback(e);
}

void HoverEnemyState::OnAttackExecute(Enemy* e)
{
    const float chaseSpeed = e->m_speed * 4.f;

    // Get player transform
    Entity player = *e->GetPlayerEntity();
    if (!e->m_entityManager->HasComponent<Transform>(player))
    {
        return;
    }

    Transform& enemyTf = e->m_entityManager->GetComponent<Transform>(e->m_selfEntity);
    Transform& playerTf = e->m_entityManager->GetComponent<Transform>(player);

    Vector3f toPlayer = playerTf.GetPosition() - enemyTf.GetPosition();

    auto* body = e->m_entityManager->GetComponent<Physics::RigidBody*>(e->m_selfEntity);

    if (e->IsAnimationEnded()) 
    {
        // Switch to flee state once attack animation finishes
        e->GetFSM()->ChangeState(FleeState<Enemy>::Instance());
    }
    else if (body)
    {
        // Compute force towards player
        Vector3f toPlayerDir = Normalize(toPlayer);
        Vector3f targetVel = toPlayerDir * chaseSpeed;
        Vector3f curVel = body->GetLinearVelocity();
        Vector3f diff = targetVel - curVel;
        float forceScale = body->GetTotalMass() * 20.0f;
        Vector3f force = diff * forceScale;

        body->ApplyLinearForce(force);

        // Face toward horizontal movement direction
        Vector3f horizVel = Vector3f(curVel.x, 0.f, curVel.z);
        if (Length(horizVel) > 0.01f) 
        {
            Quaternion orientation = body->GetOrientation();
            Vector3f euler = QuaternionToEulerDegrees(orientation);
            float currentYaw = euler.z + 90.f;

            float targetYaw = std::atan2(horizVel.x, horizVel.z) * 180.f / PI;
            float deltaYaw = targetYaw - currentYaw;

            // Wrap deltaYaw to [-180, 180]
            while (deltaYaw > 180.f)
            {
                deltaYaw -= 360.f;
            }
            while (deltaYaw < -180.f)
            {
                deltaYaw += 360.f;
            }

            float rotationSpeed = 5.0f;
            float angularVelY = deltaYaw * rotationSpeed * PI / 180.f;
            body->SetAngularVelocity(Vector3f(0.f, 0.f, angularVelY));
        }
    }
}

void HoverEnemyState::OnAttackExit(Enemy* e)
{
    // Reset timer and cleanup collision callback
    e->m_timer = 0.f;
    CleanupAttackCallback(e);
}

void HoverEnemyState::OnFleeEnter(Enemy* e)
{
    // Play pain animation to indicate damage and start fleeing
    e->SetAnimation("pain");
    e->m_timer = 0.f;
}

void HoverEnemyState::OnFleeExecute(Enemy* e)
{
    const float fleeSpeed = e->m_speed * 1.2f;
    const float verticalSpeed = 3.5f;

    Entity player = *e->GetPlayerEntity();
    if (!e->m_entityManager->HasComponent<Transform>(player))
    {
        return;
    }

    Transform& enemyTf = e->m_entityManager->GetComponent<Transform>(e->m_selfEntity);
    Transform& playerTf = e->m_entityManager->GetComponent<Transform>(player);

    Vector3f toPlayer = playerTf.GetPosition() - enemyTf.GetPosition();
    Vector3f awayFromPlayer = -Normalize(toPlayer);

    auto* body = e->m_entityManager->GetComponent<Physics::RigidBody*>(e->m_selfEntity);

    // Track time fleeing
    e->m_timer += 1.0f / 60.0f;

    if (e->m_timer >= 5.0f) 
    {
        // After fleeing for 5 seconds, resume chase
        e->GetFSM()->ChangeState(ChaseState<Enemy>::Instance());
    }
    else if (body)
    {
        // Flee away from player while gaining altitude
        Vector3f targetVel = (awayFromPlayer * fleeSpeed) + Vector3f(0.f, verticalSpeed, 0.f);
        Vector3f curVel = body->GetLinearVelocity();
        Vector3f diff = targetVel - curVel;
        float forceScale = body->GetTotalMass() * 10.0f;
        Vector3f force = diff * forceScale;
        body->ApplyLinearForce(force);

        // Face toward the player as it flees
        Vector3f lookDir = Normalize(Vector3f(toPlayer.x, 0.f, toPlayer.z));
        if (Length(lookDir) > 0.01f) 
        {
            Quaternion orientation = body->GetOrientation();
            Vector3f euler = QuaternionToEulerDegrees(orientation);
            float currentYaw = euler.z + 90.f;

            float targetYaw = std::atan2(lookDir.x, lookDir.z) * 180.f / PI;
            float deltaYaw = targetYaw - currentYaw;

            while (deltaYaw > 180.f)
            {
                deltaYaw -= 360.f;
            }
            while (deltaYaw < -180.f)
            {
                deltaYaw += 360.f;
            }

            float rotationSpeed = 5.0f;
            float angularVelZ = deltaYaw * rotationSpeed * PI / 180.f;
            body->SetAngularVelocity(Vector3f(0.f, 0.f, angularVelZ));
        }
    }
}

void HoverEnemyState::OnFleeExit(Enemy* e)
{
    // Reset flee timer
    e->m_timer = 0.f;
}

void HoverEnemyState::OnPatrolEnter(Enemy* e)
{
    // Idle animation for patrolling Hover enemy
    e->SetAnimation("stand");
    e->m_timer = 0.f;
}

void HoverEnemyState::OnPatrolExecute(Enemy* e)
{
    float speed = e->m_speed;
    auto* body = e->m_entityManager->GetComponent<Physics::RigidBody*>(e->m_selfEntity);

    e->m_timer += 1.0f / 60.0f;

    // Choose a new direction every 10 seconds
    if (e->m_timer >= 10.0f) 
    {
        float angle = (std::rand() % 360) * PI / 180.f;
        e->m_direction = Vector3f(std::cos(angle), 0.f, std::sin(angle));
        e->m_timer = 0.f;
    }

    // If player is nearby, transition to chase state
    if (e->IsPlayerInRange(e, 80.0f)) 
    {
        e->GetFSM()->ChangeState(ChaseState<Enemy>::Instance());
        e->AlertNearbyEnemies();
        return;
    }

    if (body) 
    {
        // Hover up/down using a sinusoidal oscillation
        float amplitude = 1.5f;
        float frequency = 0.1f;
        float omega = 2.0f * PI * frequency;
        float hoverHeight = 70.5f;

        Vector3f targetVel = e->m_direction * speed;
        float targetY = hoverHeight + amplitude * std::sin(omega * e->m_timer);

        Vector3f curPos = body->GetPosition();
        Vector3f curVel = body->GetLinearVelocity();

        float stiffness = 20.0f;
        float damping = 10.0f;
        float displacement = targetY - curPos.y;
        float springForceY = stiffness * displacement - damping * curVel.y;

        body->ApplyLinearForce(Vector3f(0.f, springForceY, 0.f));

        Vector3f diff = Vector3f(targetVel.x - curVel.x, 0.f, targetVel.z - curVel.z);
        float forceScale = body->GetTotalMass() * 10.0f;
        Vector3f force = diff * forceScale;

        body->ApplyLinearForce(force);

        // Rotate to face horizontal velocity
        Vector3f horizVel = Vector3f(curVel.x, 0.f, curVel.z);
        if (Length(horizVel) > 0.01f) 
        {
            Quaternion orientation = body->GetOrientation();
            Vector3f euler = QuaternionToEulerDegrees(orientation);
            float currentYaw = euler.z + 90.f;

            float targetYaw = std::atan2(horizVel.x, horizVel.z) * 180.f / PI;
            float deltaYaw = targetYaw - currentYaw;

            while (deltaYaw > 180.f)
            {
                deltaYaw -= 360.f;
            }
            while (deltaYaw < -180.f)
            {
                deltaYaw += 360.f;
            }

            float rotationSpeed = 5.0f;
            float angularVelY = deltaYaw * rotationSpeed * PI / 180.f;
            body->SetAngularVelocity(Vector3f(0.f, 0.f, angularVelY));
        }
    }
}

void HoverEnemyState::OnPatrolExit(Enemy* e)
{
    // Reset patrol timer
    e->m_timer = 0.f;
}

void HoverEnemyState::OnChaseEnter(Enemy* e)
{
    // Start takeoff animation before chasing
    e->SetAnimation("takeof");
}

void HoverEnemyState::OnChaseExecute(Enemy* e)
{
    const float chaseSpeed = e->m_speed * 1.5f;
    const float maxChaseDistance = 170.0f;
    const float attackRange = 30.0f;

    Entity player = *e->GetPlayerEntity();
    if (!e->m_entityManager->HasComponent<Transform>(player))
    {
        return;
    }

    Transform& enemyTf = e->m_entityManager->GetComponent<Transform>(e->m_selfEntity);
    Transform& playerTf = e->m_entityManager->GetComponent<Transform>(player);
    Vector3f toPlayer = playerTf.GetPosition() - enemyTf.GetPosition();
    float distance = Length(toPlayer);

    // If player is too far, resume patrol
    if (distance > maxChaseDistance) 
    {
        e->GetFSM()->ChangeState(PatrolState<Enemy>::Instance());
        return;
    }

    // If close enough, start attack
    if (distance <= attackRange) 
    {
        e->GetFSM()->ChangeState(AttackState<Enemy>::Instance());
        return;
    }

    auto* body = e->m_entityManager->GetComponent<Physics::RigidBody*>(e->m_selfEntity);
    if (body) 
    {
        Vector3f toPlayerDir = Normalize(toPlayer);
        Vector3f targetVel = toPlayerDir * chaseSpeed;
        Vector3f curVel = body->GetLinearVelocity();
        Vector3f diff = targetVel - curVel;
        float forceScale = body->GetTotalMass() * 20.0f;
        Vector3f force = diff * forceScale;

        body->ApplyLinearForce(force);

        Vector3f horizVel = Vector3f(curVel.x, 0.f, curVel.z);
        if (Length(horizVel) > 0.01f) 
        {
            Quaternion orientation = body->GetOrientation();
            Vector3f euler = QuaternionToEulerDegrees(orientation);
            float currentYaw = euler.z + 90.f;

            float targetYaw = std::atan2(horizVel.x, horizVel.z) * 180.f / PI;
            float deltaYaw = targetYaw - currentYaw;

            while (deltaYaw > 180.f)
            {
                deltaYaw -= 360.f;
            }
            while (deltaYaw < -180.f)
            {
                deltaYaw += 360.f;
            }

            float rotationSpeed = 5.0f;
            float angularVelY = deltaYaw * rotationSpeed * PI / 180.f;
            body->SetAngularVelocity(Vector3f(0.f, 0.f, angularVelY));
        }
    }
}

void HoverEnemyState::OnChaseExit(Enemy* e)
{
    // Reset chase timer
    e->m_timer = 0.f;
}

void HoverEnemyState::OnDeathEnter(Enemy* e)
{
    // Play death animation
    e->SetAnimation("death");
}

void HoverEnemyState::OnDeathExecute(Enemy* e)
{
    // After death animation ends, reset to patrol (for looping test enemies)
    if (e->IsAnimationEnded()) 
    {
        e->GetFSM()->ChangeState(PatrolState<Enemy>::Instance());
    }
}

void HoverEnemyState::OnDeathExit(Enemy* e)
{
    // Add Dead tag to mark for cleanup
    e->m_entityManager->AddComponent<Dead>(e->m_selfEntity, Dead());
    e->m_timer = 0.f;
}

void HoverEnemyState::OnPainEnter(Enemy* e)
{
    // Play pain animation on hit
    e->SetAnimation("pain");
}

void HoverEnemyState::OnPainExecute(Enemy* e)
{
    e->m_timer += 1.0f / 60.0f;

    // After short delay, resume chase
    if (e->m_timer >= 6.0f) 
    {
        e->GetFSM()->ChangeState(ChaseState<Enemy>::Instance());
    }
}

void HoverEnemyState::OnPainExit(Enemy* e)
{
    // Reset timer on pain exit
    e->m_timer = 0.f;
}