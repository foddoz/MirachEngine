#include "attach_collision_callback.hpp"

AttachCollisionCallback::AttachCollisionCallback(Physics::RigidBody* rb, EntityManager* em, PhysicsSystem* ps)
	: 
    CollisionCallback(rb), 
    m_entityManager(em), 
    m_physicsSystem(ps)
{

}

void AttachCollisionCallback::OnContactEnter(Physics::RigidBody* other, const Physics::ContactManifold& cm)
{
    Vector3f point = cm.points[0].position;
    Vector3f angularVel = m_body->GetAngularVelocity();

    Vector3f axisDir = Vector3f(0.f, 1.f, 0.f);
    axisDir = Normalize(axisDir); //Normalise axis direction just in case
    float shortestDistance = Length(Cross(point - m_body->GetPosition(), axisDir));

    // === Output contact info === //
    std::cout << "\nContact Point: " << point.x << ", " << point.y << ", " << point.z << "\n";
    std::cout << "Angular Velocity: " << angularVel.x << ", " << angularVel.y << ", " << angularVel.z << "\n";
    std::cout << "Shortest Distance: " << shortestDistance << "\n";

    Entity otherEntity = m_entityManager->FindEntityByComponent(other);

    Transform &tf = m_entityManager->GetComponent<Transform>(otherEntity);
    float radius = tf.GetScale().x;
    float mass = other->GetTotalMass();

    // === Calculate local position and orientation of the projectile to the target === //
    const Matrix3f Ra = ToMat3(m_body->GetOrientation());
    const Matrix3f RaT = Transpose(Ra);
    Vector3f localOffset = RaT * (other->GetPosition() - m_body->GetPosition());
    Quaternion localRot = Inverse(m_body->GetOrientation()) * other->GetOrientation();

    Physics::Collider* col = m_physicsSystem->GetWorld()->AddSphereCollider(m_body, radius, mass, localOffset, localRot); //Add projectile collider to the target

    m_collidedBodies.erase(other);
    m_physicsSystem->DestroyEntity(otherEntity);
    m_entityManager->AddComponent<Physics::Collider*>(otherEntity, col);
}
void AttachCollisionCallback::OnContactPersist(Physics::RigidBody* other, const Physics::ContactManifold& cm)
{

}
void AttachCollisionCallback::OnContactExit(Physics::RigidBody* other)
{

}
