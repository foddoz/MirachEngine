#include "log_collision_callback.hpp"

LogCollisionCallback::LogCollisionCallback(Physics::RigidBody* rb)
	: 
	CollisionCallback(rb)
{

}

void LogCollisionCallback::OnContactEnter(Physics::RigidBody* rb, const Physics::ContactManifold& cm)
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
}
void LogCollisionCallback::OnContactPersist(Physics::RigidBody* rb, const Physics::ContactManifold& cm)
{

}
void LogCollisionCallback::OnContactExit(Physics::RigidBody* rb)
{

}