#include "scratch_rigidbody_collider.hpp"

namespace Physics::Backend
{
    SimpleCollider::SimpleCollider(RigidBody* body, const Vector3f& localPos, const Quaternion& localRot)
        : 
        m_localPosition(localPos), 
        m_localRotation(localRot), 
        m_friction(0.8f), 
        m_bounciness(0.5f), 
        m_category(0), 
        m_mask(0)
    {

        SimpleRigidBody* scratchBody = static_cast<SimpleRigidBody*>(body);
        scratchBody->AddCollider(this);
        m_parentBody = scratchBody;
    }

    void SimpleCollider::SetCollisionMask(uint16_t m)
    {
        m_mask = m;
    }

    void SimpleCollider::SetFriction(float f)
    {
        m_friction = f;
    }

    float SimpleCollider::GetFriction() const
    {
        return m_friction;
    }

    void SimpleCollider::SetBounciness(float b)
    {
        m_bounciness = b;
    }

    float SimpleCollider::GetBounciness() const
    {
        return m_bounciness;
    }

    void SimpleCollider::SetCategoryBits(uint16_t c)
    {
        m_category = c;
    }

    uint16_t SimpleCollider::GetCategoryBits() const
    {
        return m_category;
    }

    void SimpleCollider::SetMass(float m)
    {
        if (m <= 0.f)
        {
            m = 1.f;
        }

        m_mass = m;
        m_parentBody->AddMass(m_mass);
    }

    float SimpleCollider::GetMass() const
    {
        return m_mass;
    }

    void SimpleCollider::AddMass(float m)
    {
        m_mass += m;
    }

    Vector3f SimpleCollider::GetWorldCentre()
    {
        SimpleRigidBody* body = m_parentBody;
        Matrix3f Rb = ToMat3(body->GetOrientation());
        //return body->m_position + (Rb * m_localPosition);
        return body->GetPosition() + (Rb * m_localPosition);
    }

    Matrix3f SimpleCollider::GetCompositeRotation()
    {
        // Composite rotation (body * collider local)
        Quaternion qB = Normalize(m_parentBody->GetOrientation());
        Quaternion qC = Normalize(m_localRotation);
        return ToMat3(qB) * ToMat3(qC);
    }

    RigidBody* SimpleCollider::GetParentBody()
    {
        return m_parentBody;
    }

    void SimpleCollider::CalculateBroadphaseAABBThroughExtents(Vector3f halfExtents)
    {
        Vector3f centre = GetWorldCentre();
        m_broadphaseAABB.min = centre - halfExtents;
        m_broadphaseAABB.max = centre + halfExtents;

        //m_parentBody->m_broadphaseAABB.Expand(m_broadphaseAABB);
        m_parentBody->BroadphaseExpand(m_broadphaseAABB);
    }

    ShapeType SimpleCollider::GetShapeType() const
    {
        return m_shapeType;
    }

    Vector3f SimpleCollider::GetLocalPosition() const
    {
        return m_localPosition;
    }
    
    void SimpleCollider::AdjustLocalPositionToCOM(const Vector3f& com)
    {
        m_localPosition -= com;
    }

    Quaternion SimpleCollider::GetLocalRotation() const
    {
        return m_localRotation;
    }

    AABB SimpleCollider::GetBroadphaseAABB() const
    {
        return m_broadphaseAABB;
    }
}