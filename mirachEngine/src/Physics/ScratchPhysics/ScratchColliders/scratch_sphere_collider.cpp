#include "../scratch_rigidbody_collider.hpp"

namespace Physics::Backend
{
    SphereCollider::SphereCollider(RigidBody* body, float mass, float radius, const Vector3f& localPos, const Quaternion& localRot)
        : 
        SimpleCollider(body, localPos, localRot), 
        m_radius(radius)
    {
        m_shapeType = ShapeType::SPHERE;

        SetMass(mass);
        RecomputeBroadPhaseAABB(0.2f);
    }

    void SphereCollider::RecomputeBroadPhaseAABB(float margin)
    {
        // Rotation irrelevant for sphere; ensure it’s set (AddSphereCollider sets it once,
        // but keeping it here makes it robust if radius changes)
        Vector3f e = Vector3f(m_radius + margin);
        CalculateBroadphaseAABBThroughExtents(e);
    }

    bool SphereCollider::RayIntersect(const Ray& ray, float& outT)
    {
        Vector3f center = GetWorldCentre();

        Vector3f oc = ray.origin - center;
        float a = glm::dot(ray.direction, ray.direction);
        float b = 2.0f * Dot(oc, ray.direction);
        float c = glm::dot(oc, oc) - m_radius * m_radius;
        float discriminant = b * b - 4 * a * c;

        if (discriminant < 0) 
        {
            return false;
        }
        else 
        {
            float sqrtDisc = sqrt(discriminant);
            float t1 = (-b - sqrtDisc) / (2.0f * a);
            float t2 = (-b + sqrtDisc) / (2.0f * a);

            // Choose nearest positive t
            if (t1 >= 0) 
            {
                outT = t1;
                return true;
            }
            else if (t2 >= 0) 
            {
                outT = t2;
                return true;
            }
            else 
            {
                return false;
            }
        }
    }

    Matrix3f SphereCollider::CalculateInertiaTensor()
    {
        float inertiaScalar = (2.0f / 5.0f) * m_mass * m_radius * m_radius;

        Matrix3f inertiaTensor = Identity3x3() * inertiaScalar;
        return inertiaTensor;
    }

    float SphereCollider::GetRadius() const
    {
        return m_radius;
    }
}