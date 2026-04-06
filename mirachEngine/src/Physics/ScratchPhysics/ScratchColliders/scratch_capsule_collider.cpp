#include "../scratch_rigidbody_collider.hpp"    

namespace Physics::Backend
{
    CapsuleCollider::CapsuleCollider(RigidBody* body, const Vector3f& localPos, float mass, float radius, float cylinderLength)
        : 
        SimpleCollider(body, localPos, Quaternion(1.f, 0.f, 0.f, 0.f)), 
        m_radius(radius), 
        m_cylinderLength(cylinderLength)
    {
        m_shapeType = ShapeType::CAPSULE;

        SetMass(mass);
        RecomputeBroadPhaseAABB(0.2f);
    }

    void CapsuleCollider::RecomputeBroadPhaseAABB(float margin)
    {
        Matrix3f R = GetCompositeRotation();

        const float halfH = 0.5f * m_cylinderLength;
        // rotate the axis
        Vector3f a = R * Vector3f(0.f, halfH, 0.f);
        // |a| is the axis projection on world axes; add radius in all directions
        Vector3f e = Vector3f(std::abs(a.x), std::abs(a.y), std::abs(a.z))
            + Vector3f(m_radius + margin);

        CalculateBroadphaseAABBThroughExtents(e);
    }

    bool CapsuleCollider::RayIntersect(const Ray& ray, float& outT)
    {
        return false;
    }

    Matrix3f CapsuleCollider::CalculateInertiaTensor()
    {
        float r = m_radius;
        float h = m_cylinderLength;

        float cylVolume = PI * r * r * h;
        float hemiVolume = (2.f * PI * r * r * r) / 3.f;

        float cylMass = (m_mass * cylVolume) / (cylVolume + hemiVolume * 2.f);
        float hemiMass = (m_mass * hemiVolume) / (cylVolume + hemiVolume * 2.f);

        Vector3f cylinderI;
        cylinderI.x = (1.0f / 12.0f) * cylMass * (3 * r * r + h * h);
        cylinderI.y = 0.5f * cylMass * r * r;
        cylinderI.z = cylinderI.x;

        Vector3f hemisphereI;
        hemisphereI.x = (83.f / 320.f) * hemiMass * r * r;
        hemisphereI.y = (2.f / 5.f) * hemiMass * r * r;
        hemisphereI.z = hemisphereI.x;

        float d = h / 2.f + (3 * r) / 8.f;

        float Ixx = cylinderI.x + 2.f * (hemisphereI.x + hemiMass * d * d);
        float Iyy = cylinderI.y + 2.f * (hemisphereI.y);
        float Izz = Ixx;

        Matrix3f inertiaTensor = Diagonal3x3(Vector3f(Ixx, Iyy, Izz));
        return inertiaTensor;
    }
}