#include "physics_system.hpp"

PhysicsSystem::PhysicsSystem(EntityManager* manager)
    : 
    m_entityManager(manager), 
    m_world(Physics::CreatePhysicsWorld()) 
{

}

void PhysicsSystem::Reset(EntityManager* newManager)
{
    m_world.reset(); // properly destroys old world
    m_entityManager = newManager;
    m_world = Physics::CreatePhysicsWorld(); // Recreate a new world (must return unique_ptr)
}

void PhysicsSystem::DestroyEntity(Entity e)
{
    if (m_entityManager->HasComponent<Physics::RigidBody*>(e))
    {
        Physics::RigidBody* rb = m_entityManager->GetComponent<Physics::RigidBody*>(e);

        m_world->RemoveRigidBody(rb);

        m_entityManager->RemoveComponent<Physics::RigidBody*>(e);
        m_entityManager->RemoveComponent<Physics::Collider*>(e);
    }
}

void PhysicsSystem::Update(float deltaTime)
{
    m_world->Update(deltaTime);

    const auto& rigidbodies = m_entityManager->GetAllComponents<Physics::RigidBody*>();
    for (const auto& [entity, rb] : rigidbodies) 
    {
        if (!rb || !m_entityManager->HasComponent<Transform>(entity))
        {
            continue;
        }

        Transform& tf = m_entityManager->GetComponent<Transform>(entity);
        Vector3f pos = rb->GetWorldCentreOfMass();
        tf.SetPosition(pos);

        Quaternion q = rb->GetOrientation();

        if (!tf.IsYAxisUp()) 
        {
            // Apply rotation offset for Z-up coordinate systems
            Quaternion offset = EulerDegreesToQuaternion(Vector3f(-90.f, 0.f, 0.f));
            q = offset * q;
        }

        tf.SetRotation(q);
    }
}

Physics::World* PhysicsSystem::GetWorld() const 
{ 
    return m_world.get(); 
}

EntityManager* PhysicsSystem::GetEntityManagerPtr() const
{
    return m_entityManager;
}