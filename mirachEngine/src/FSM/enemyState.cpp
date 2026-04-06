/**
 * @file enemyState.cpp
 * @brief Implements enemy FSM behavior logic for all states: Attack, Flee, Patrol, Chase, Death, and Pain.
 *
 * Each method controls state-specific behavior such as animation triggering, movement, and transitions.
 * Uses Entity-Component-System (ECS), physics, and animation systems to perform AI logic.
 */

#include "enemyState.hpp"

 /**
  * @brief Animation index reference.
  * - standb: 5 to 24
  * - stand: 0 to 4
  * - walkc: 25 to 35
  * - att_b: 55 to 75
  * - att_c: 76 to 109
  * - deathc: 236 to 243
  * - run: 36 to 41
  * - painb: 203 to 222
  */

  // ======================== Attack State ========================

  /**
   * @brief Called when entering the Attack state.
   * Sets attacking flag, resets m_timer, and registers attack collision callback.
   * @param e Pointer to the enemy entity.
   */
void EnemyState::OnAttackEnter(Enemy* e) 
{
    e->m_isAttacking = true;
    e->m_timer = 0.f;
    RegisterAttackCallback(e);
}

/**
 * @brief Called every frame while attacking.
 * Plays attack animation, checks player distance for flee/chase switch,
 * and applies knockback to the player if in range after some time.
 * @param e Pointer to the enemy.
 */
void EnemyState::OnAttackExecute(Enemy* e) 
{
    Entity player = *e->GetPlayerEntity();
    e->SetAnimation("att_c");

    if (e->IsPlayerInRange(e, 50.0f) && e->IsHealthLow(30)) 
    {
        e->GetFSM()->ChangeState(FleeState<Enemy>::Instance());
        return;
    }

    if (!e->IsPlayerInRange(e, 10.0f)) 
    {
        e->GetFSM()->ChangeState(ChaseState<Enemy>::Instance());
        return;
    }

    e->m_timer += 1.0f / 60.0f;
    if (e->m_timer >= 4.0f) 
    {
        if (!e->m_entityManager->HasComponent<Physics::RigidBody*>(player))
        {
            return;
        }

        Physics::RigidBody* playerBody = e->m_entityManager->GetComponent<Physics::RigidBody*>(player);
        Transform& enemyTf = e->m_entityManager->GetComponent<Transform>(e->m_selfEntity);
        Transform& playerTf = e->m_entityManager->GetComponent<Transform>(player);

        Vector3f knockDir = Normalize(playerTf.GetPosition() - enemyTf.GetPosition());
        Vector3f knockForce = knockDir * Vector3f(0.1f, 0.1f, 0);
        playerBody->ApplyLinearImpulse(knockForce);
    }
}

/**
 * @brief Called when exiting the Attack state.
 * Clears attack flag and unregisters collision callback.
 * @param e Pointer to the enemy.
 */
void EnemyState::OnAttackExit(Enemy* e) 
{
    e->m_isAttacking = false;
    CleanupAttackCallback(e);
}

// ======================== Flee State ========================

/**
 * @brief Called when entering the Flee state.
 * Plays hurt animation and resets m_timer.
 */
void EnemyState::OnFleeEnter(Enemy* e) 
{
    e->SetAnimation("painb");
    e->m_timer = 0.f;
}

/**
 * @brief Called every frame during Flee state.
 * Applies force to move away from the player and rotates to face opposite direction.
 */
void EnemyState::OnFleeExecute(Enemy* e) 
{
    float fleeSpeed = e->m_speed * 1.2f;
    auto* body = e->m_entityManager->GetComponent<Physics::RigidBody*>(e->m_selfEntity);
    e->SetAnimation("run");

    Entity player = *e->GetPlayerEntity();
    if (!e->m_entityManager->HasComponent<Transform>(player))
    {
        return;
    }

    Transform& playerTf = e->m_entityManager->GetComponent<Transform>(player);
    Transform& selfTf = e->m_entityManager->GetComponent<Transform>(e->m_selfEntity);

    Vector3f toPlayer = playerTf.GetPosition() - selfTf.GetPosition();
    float distance = Length(Vector3f(toPlayer.x, 0.f, toPlayer.z));

    if (e->IsHealthLow(1)) 
    {
        e->GetFSM()->ChangeState(DeathState<Enemy>::Instance());
        return;
    }

    if (distance > 100.f) 
    {
        e->GetFSM()->ChangeState(PatrolState<Enemy>::Instance());
        return;
    }

    Vector3f fleeDir = -Normalize(Vector3f(toPlayer.x, 0.f, toPlayer.z));

    if (body) 
    {
        Vector3f targetVel = fleeDir * fleeSpeed;
        Vector3f curVel = body->GetLinearVelocity();
        Vector3f diff = Vector3f(targetVel.x - curVel.x, 0.f, targetVel.z - curVel.z);
        float forceScale = body->GetTotalMass() * 10.0f;
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
            float angularVelY = deltaYaw * 5.0f * PI / 180.f;
            body->SetAngularVelocity(Vector3f(0.f, 0.f, angularVelY));
        }
    }
}

/**
 * @brief Called when exiting the Flee state.
 * Resets internal flee m_timer.
 */
void EnemyState::OnFleeExit(Enemy* e) 
{
    e->m_timer = 0.f;
}

// ======================== Patrol State ========================

/**
 * @brief Called when entering the Patrol state.
 * Starts walking animation and resets m_timer.
 */
void EnemyState::OnPatrolEnter(Enemy* e) 
{
    e->SetAnimation("walkc");
    e->m_timer = 0.f;
}

/**
 * @brief Called every frame during Patrol.
 * Enemy wanders around, checks for player, and may switch to Chase or Flee.
 */
void EnemyState::OnPatrolExecute(Enemy* e) 
{
    float speed = e->m_speed;
    auto* body = e->m_entityManager->GetComponent<Physics::RigidBody*>(e->m_selfEntity);
    e->m_timer += 1.0f / 60.0f;

    if (e->IsHealthLow(1)) 
    {
        e->GetFSM()->ChangeState(DeathState<Enemy>::Instance());
        return;
    }

    if (e->IsPlayerInRange(e, 50.0f)) 
    {
        if (e->IsHealthLow(100)) 
        {
            e->GetFSM()->ChangeState(FleeState<Enemy>::Instance());
            return;
        }
        else 
        {
            e->GetFSM()->ChangeState(ChaseState<Enemy>::Instance());
            e->AlertNearbyEnemies();
            return;
        }
    }

    if (e->m_timer >= 20.0f) 
    {
        float angle = (std::rand() % 360) * PI / 180.f;
        e->m_direction = Vector3f(std::cos(angle), 0.f, std::sin(angle));
        e->m_timer = 0.f;
    }

    if (body) 
    {
        Vector3f targetVel = e->m_direction * speed;
        Vector3f curVel = body->GetLinearVelocity();
        Vector3f diff = Vector3f(targetVel.x - curVel.x, 0.f, targetVel.z - curVel.z);
        Vector3f force = diff * (body->GetTotalMass() * 10.0f);
        body->ApplyLinearForce(force);

        Vector3f horizVel = Vector3f(curVel.x, 0.f, curVel.z);
        if (Length(horizVel) > 0.01f) 
        {
            Quaternion orientation = body->GetOrientation();
            Vector3f euler = QuaternionToEulerDegrees(orientation);
            float currentYaw = euler.y;

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

            if (std::abs(deltaYaw) > 1.0f) 
            {
                float rotationSpeed = 5.0f;
                float angularVelY = deltaYaw * rotationSpeed * PI / 180.f;
                angularVelY = std::clamp(angularVelY, -2.f, 2.f);
                body->SetAngularVelocity(Vector3f(0.f, 0.f, angularVelY));
            }
            else 
            {
                body->SetAngularVelocity(Vector3f(0.f, 0.f, 0.f));
                body->SetAngularFreeAxis(Vector3f(1.f, 1.f, 0.f));
            }
        }
    }
}

/**
 * @brief Called when exiting the Patrol state.
 */
void EnemyState::OnPatrolExit(Enemy* e) 
{
    e->m_timer = 0.f;
}

// ======================== Chase State ========================

/**
 * @brief Called when entering the Chase state.
 * Sets run animation.
 */
void EnemyState::OnChaseEnter(Enemy* e) 
{
    e->SetAnimation("run");
}

/**
 * @brief Called every frame while chasing the player.
 * Switches to Attack/Flee/Patrol based on range and health.
 */
void EnemyState::OnChaseExecute(Enemy* e) 
{
    const float chaseSpeed = e->m_speed * 1.5f;
    Entity player = *e->GetPlayerEntity();
    if (!e->m_entityManager->HasComponent<Transform>(player))
    {
        return;
    }

    Transform& enemyTf = e->m_entityManager->GetComponent<Transform>(e->m_selfEntity);
    Transform& playerTf = e->m_entityManager->GetComponent<Transform>(player);
    Vector3f toPlayer = playerTf.GetPosition() - enemyTf.GetPosition();
    toPlayer.y = 0.0f;
    float distance = Length(toPlayer);

    if (e->IsHealthLow(1)) 
    {
        e->GetFSM()->ChangeState(DeathState<Enemy>::Instance());
        return;
    }

    if (e->IsPlayerInRange(e, 50.0f) && e->IsHealthLow(100)) 
    {
        e->GetFSM()->ChangeState(FleeState<Enemy>::Instance());
        return;
    }

    if (distance > e->m_maxChaseDistance) 
    {
        e->GetFSM()->ChangeState(PatrolState<Enemy>::Instance());
        return;
    }

    if (distance <= e->m_attackRange) 
    {
        e->GetFSM()->ChangeState(AttackState<Enemy>::Instance());
        return;
    }

    Vector3f direction = Normalize(toPlayer);
    e->m_direction = direction;

    auto* body = e->m_entityManager->GetComponent<Physics::RigidBody*>(e->m_selfEntity);
    if (body) 
    {
        Vector3f targetVel = e->m_direction * chaseSpeed;
        Vector3f curVel = body->GetLinearVelocity();
        Vector3f diff = Vector3f(targetVel.x - curVel.x, 0.f, targetVel.z - curVel.z);
        float forceScale = body->GetTotalMass() * 10.0f;
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

/**
 * @brief Called when exiting the Chase state.
 */
void EnemyState::OnChaseExit(Enemy* e) 
{
    e->m_timer = 0.f;
}

// ======================== Death State ========================

/**
 * @brief Called when entering the Death state.
 * Triggers death animation.
 */
void EnemyState::OnDeathEnter(Enemy* e) 
{
    e->SetAnimation("death");
}

/**
 * @brief Called every frame while in Death state.
 * Applies gravity and switches to Patrol if animation ends.
 */
void EnemyState::OnDeathExecute(Enemy* e) 
{
    Physics::RigidBody* body = e->m_entityManager->GetComponent<Physics::RigidBody*>(e->m_selfEntity);
    Vector3f gravity = Vector3f(0.0f, -body->GetTotalMass() * 9.8f, 0.0f);
    body->ApplyLinearForce(gravity);

    if (e->IsAnimationEnded()) 
    {
        e->GetFSM()->ChangeState(PatrolState<Enemy>::Instance());
    }
}

/**
 * @brief Called when exiting the Death state.
 * Adds Dead component and resets m_timer.
 */
void EnemyState::OnDeathExit(Enemy* e) 
{
    e->m_entityManager->AddComponent<Dead>(e->m_selfEntity, Dead());
    e->m_timer = 0.f;
}

// ======================== Pain State ========================

/**
 * @brief Called when entering the Pain state.
 * Plays pain animation.
 */
void EnemyState::OnPainEnter(Enemy* e) 
{
    e->SetAnimation("painb");
}

/**
 * @brief Called every frame in the Pain state.
 * After 6 seconds, switches back to Patrol.
 */
void EnemyState::OnPainExecute(Enemy* e) 
{
    e->m_timer += 1.0f / 60.0f;
    if (e->m_timer >= 6.0f) 
    {
        e->GetFSM()->ChangeState(PatrolState<Enemy>::Instance());
    }
}

/**
 * @brief Called when exiting the Pain state.
 * Resets m_timer.
 */
void EnemyState::OnPainExit(Enemy* e) 
{
    e->m_timer = 0.f;
}
