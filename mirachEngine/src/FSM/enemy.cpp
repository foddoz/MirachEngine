#include "enemy.h"
#include "IEnemyStatelogic.h"

#include <iostream>

Enemy::Enemy(const std::string& name, Entity self, Entity* player, EntityManager* mgr, IEnemyStateLogic* AILogic, PhysicsSystem* physics)
    : 
    m_name(name), 
    m_selfEntity(self), 
    m_playerEntity(player), 
    m_entityManager(mgr), 
    m_logic(AILogic), 
    m_physicsSystem(physics)
{
    //Initialized state machine, the AI logic script
    m_fsm = new StateMachine<Enemy>(this);
    //Set default state
    m_fsm->ChangeState(PatrolState<Enemy>::Instance());
}

Enemy::Enemy()
    : 
    m_name("Unnamed"), 
    m_logic(nullptr), 
    m_fsm(nullptr),
    m_selfEntity(0), 
    m_playerEntity(nullptr), 
    m_entityManager(nullptr)
{

}

Enemy::~Enemy() 
{
    if (m_fsm) 
    {
        delete m_fsm;
        m_fsm = nullptr;
    }
}

std::string Enemy::GetName() const 
{
    return m_name;
}

float Enemy::GetSpeed() const 
{
    return m_speed;
}

EntityManager* Enemy::GetEntityManager() const 
{
    return m_entityManager;
}

Entity Enemy::GetSelfEntity() const 
{
    return m_selfEntity;
}

Entity* Enemy::GetPlayerEntity() const 
{
    return m_playerEntity;
}

void Enemy::SetSpeed(float s) 
{
    m_speed = s;
}

StateMachine<Enemy>* Enemy::GetFSM() const 
{
    return m_fsm;
}

void Enemy::Update(float dt) 
{
    if (m_fsm && m_selfEntity != INVALID_ENTITY) 
    {
        m_fsm->Update();
    }
}

void Enemy::SetAnimation(const std::string& animName) 
{
    if (m_entityManager && m_entityManager->HasComponent<MD2Animator>(m_selfEntity)) 
    {
        MD2Animator& anim = m_entityManager->GetComponent<MD2Animator>(m_selfEntity);
        anim.currentAnim = animName;
    }
}

bool Enemy::IsAnimationEnded() const 
{
    if (m_entityManager && m_entityManager->HasComponent<MD2Animator>(m_selfEntity)) 
    {
        const MD2Animator& anim = m_entityManager->GetComponent<MD2Animator>(m_selfEntity);
        if (anim.mesh) 
        {
            const AnimationRange& range = anim.mesh->GetAnimationRange(anim.currentAnim);
            return anim.currentFrame == range.endFrame;
        }
    }
    return false;
}


void Enemy::FaceTarget(const Vector3f& targetPos) 
{
    Transform& tf = m_entityManager->GetComponent<Transform>(m_selfEntity);
    Vector3f currentPos = tf.GetPosition();
    Vector3f toTarget = targetPos - currentPos;
    toTarget.y = 0.0f; 

    if (Length(toTarget) < 0.001f)
    {
        return;
    }

    toTarget = Normalize(toTarget);

    float angle = std::atan2(toTarget.x, toTarget.z);
    float angularSpeed = 2.0f;

    Physics::RigidBody* rb = m_entityManager->GetComponent<Physics::RigidBody*>(m_selfEntity);
    if (rb) 
    {
        rb->SetAngularVelocity(Vector3f(0.f, angle * angularSpeed, 0.f));
    }
}

bool Enemy::IsPlayerInRange(Enemy* e, float range) 
{
    Vector3f selfPos = e->m_entityManager->GetComponent<Transform>(e->m_selfEntity).GetPosition();
    Vector3f playerPos = e->m_entityManager->GetComponent<Transform>(*e->m_playerEntity).GetPosition();

    float dx = selfPos.x - playerPos.x;
    float dz = selfPos.z - playerPos.z;

    return (std::abs(dx) <= range && std::abs(dz) <= range);
}

bool Enemy::IsHealthLow(float threshold) const 
{
    if (m_entityManager->HasComponent<Health>(m_selfEntity)) 
    {
        return m_entityManager->GetComponent<Health>(m_selfEntity).current < threshold;
    }
    return false;
}

float Enemy::GetDamage() const
{
    return m_damage;
}

void Enemy::ClearAttackCallback() 
{
    if (m_tempAttackCallback) 
    {
        m_physicsSystem->GetWorld()->UnregisterCollisionCallback(m_tempAttackCallback); // <-- NEW
        delete m_tempAttackCallback;
        m_tempAttackCallback = nullptr;
    }
}

void Enemy::AlertNearbyEnemies()
{
    if (!m_entityManager)
    {
        return;
    }

    const float alertRadius = 60.0f;

    // Get this enemy's position
    Vector3f selfPos = m_entityManager->GetComponent<Transform>(m_selfEntity).GetPosition();

    for (Entity other : m_entityManager->GetAllEntities())
    {
        if (other == m_selfEntity)
        {
            continue;
        }

        // Only check other enemies
        if (!m_entityManager->HasComponent<Enemy*>(other))
        {
            continue;
        }

        Enemy* otherEnemy = m_entityManager->GetComponent<Enemy*>(other);
        if (!otherEnemy)
        {
            continue;
        }

        // Must not already be chasing
        if (otherEnemy->GetFSM()->GetCurrentState() == ChaseState<Enemy>::Instance())
        {
            continue;
        }

        // Must be within alert radius
        Vector3f otherPos = m_entityManager->GetComponent<Transform>(other).GetPosition();
        float dist = Length(otherPos - selfPos);

        if (dist <= alertRadius)
        {
            otherEnemy->GetFSM()->ChangeState(ChaseState<Enemy>::Instance());
            //std::cout << "[Enemy] Alerted nearby enemy " << other << " to chase.\n";
        }
    }
}

PhysicsSystem* Enemy::GetPhysicsSystem() const 
{
  return m_physicsSystem;
}

void Enemy::SetAttackCallback(Physics::CollisionCallback* cb) 
{
    m_tempAttackCallback = cb;
}

// Attack
void Enemy::OnAttackEnter() 
{ 
    m_logic->OnAttackEnter(this); 
}

void Enemy::OnAttackExecute() 
{ 
    m_logic->OnAttackExecute(this); 
}

void Enemy::OnAttackExit() 
{ 
    m_logic->OnAttackExit(this); 
}

// Flee
void Enemy::OnFleeEnter() 
{ 
    m_logic->OnFleeEnter(this); 
}

void Enemy::OnFleeExecute() 
{ 
    m_logic->OnFleeExecute(this); 
}

void Enemy::OnFleeExit() 
{ 
    m_logic->OnFleeExit(this); 
}

// Patorl
void Enemy::OnPatrolEnter() 
{ 
    m_logic->OnPatrolEnter(this); 
}

void Enemy::OnPatrolExecute() 
{ 
    m_logic->OnPatrolExecute(this); 
}

void Enemy::OnPatrolExit() 
{ 
    m_logic->OnPatrolExit(this); 
}

// Chase
void Enemy::OnChaseEnter() 
{ 
    m_logic->OnChaseEnter(this); 
}

void Enemy::OnChaseExecute() 
{ 
    m_logic->OnChaseExecute(this); 
}

void Enemy::OnChaseExit() 
{ 
    m_logic->OnChaseExit(this); 
}

// Death
void Enemy::OnDeathEnter() 
{ 
    m_logic->OnDeathEnter(this); 
}

void Enemy::OnDeathExecute() 
{ 
    m_logic->OnDeathExecute(this); 
}

void Enemy::OnDeathExit() 
{ 
    m_logic->OnDeathExit(this); 
}

// Pain
void Enemy::OnPainEnter() 
{ 
    m_logic->OnPainEnter(this); 
}

void Enemy::OnPainExecute() 
{ 
    m_logic->OnPainExecute(this); 
}

void Enemy::OnPainExit() 
{ 
    m_logic->OnPainExit(this); 
}


