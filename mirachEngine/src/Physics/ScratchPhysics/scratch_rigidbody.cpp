#include "scratch_rigidbody_collider.hpp"
#include <iostream>

namespace Physics::Backend
{
    SimpleRigidBody::SimpleRigidBody(const Vector3f& position, const Quaternion& rotation, const BodyType& type)
        : 
        m_position(position), 
        m_rotation(rotation), 
        m_type(type), 
        m_angularDamping(0.05f), 
        m_angularFreeAxis(Vector3f(1.f)), 
        m_mass(0.f), 
        m_gravityEnabled(true),
        m_velocity(Vector3f(0.f)), 
        m_acceleration(Vector3f(0.f)), 
        m_netForce(Vector3f(0.f)), 
        m_angularVelocity(Vector3f(0.f)), 
        m_localCOM(Vector3f(0.f)), 
        m_inverseInertiaTensorLocal(Matrix3f(0.f)), 
        m_inverseInertiaTensorWorld(Matrix3f(0.f)),
        m_broadphaseAABB()
    {

    }

    void SimpleRigidBody::SetPosition(const Vector3f& pos)
    {
        m_position = pos;
    }

    Vector3f SimpleRigidBody::GetPosition() const
    {
        return m_position;
    }

    void SimpleRigidBody::SetOrientation(const Quaternion& rot)
    {
        m_rotation = rot;

        if (!IsDynamic())
        {
            for (int i = 0; i < m_colliders.size(); i++)
            {
                if (m_colliders[i]->GetShapeType() == ShapeType::BOX)
                {
                    BoxCollider* col = static_cast<BoxCollider*>(m_colliders[i]);
                    col->CheckStaticBoxAlignment();
                }
            }
        }
    }

    Quaternion SimpleRigidBody::GetOrientation() const
    {
        return m_rotation;
    }

    void SimpleRigidBody::ApplyLinearForce(const Vector3f& force)
    {
        if (m_type == BodyType::DYNAMIC)
        {
            m_netForce += force;
        }
    }

    void SimpleRigidBody::ApplyLinearImpulse(const Vector3f& impulse)
    {
        if (m_type == BodyType::DYNAMIC)
        {
            m_velocity += impulse / m_mass;
            /*
            std::cout << "Mass: " << m_mass << "\n";
            std::cout << "Linear velocity after collision:\n" << m_velocity.x << ", " << m_velocity.y << ", " << m_velocity.z << "\n";
            */
        }
    }

    void SimpleRigidBody::ApplyAngularImpulse(const Vector3f& impulse, const Vector3f& point)
    {
        if (m_type == BodyType::DYNAMIC)
        {
            Vector3f r = point - m_position;
            Vector3f deltaOmega = m_inverseInertiaTensorWorld * Cross(r, impulse);

            m_angularVelocity += AngularVectorToFreeAxis(deltaOmega);
        }
    }

    Vector3f SimpleRigidBody::AngularVectorToFreeAxis(const Vector3f& V)
    {
        Matrix3f Rwb = ToMat3(m_rotation);
        Matrix3f Rbw = Transpose(Rwb);

        Vector3f dOmega_local = Rbw * V; //Project to local space

        //Apply to each free axis
        dOmega_local.x *= m_angularFreeAxis.x; //0 to lock
        dOmega_local.y *= m_angularFreeAxis.y;
        dOmega_local.z *= m_angularFreeAxis.z;

        return Rwb * dOmega_local; //Back to world space
    }

    void SimpleRigidBody::SetLinearVelocity(const Vector3f& vel)
    {
        m_velocity = vel;
    }

    Vector3f SimpleRigidBody::GetLinearVelocity() const
    {
        return m_velocity;
    }

    void SimpleRigidBody::SetAngularVelocity(const Vector3f& w)
    {
        //m_angularVelocity = w;
        m_angularVelocity = AngularVectorToFreeAxis(w);
    }

    Vector3f SimpleRigidBody::GetAngularVelocity() const
    {
        return m_angularVelocity;
    }

    void SimpleRigidBody::EnableGravity(bool enabled)
    {
        m_gravityEnabled = enabled;
    }

    bool SimpleRigidBody::IsGravityEnabled() const
    {
        return m_gravityEnabled;
    }

    Vector3f SimpleRigidBody::GetNetForce() const
    {
        return m_netForce;
    }

    void SimpleRigidBody::SetNetForce(const Vector3f& netForce)
    {
        m_netForce = netForce;
    }

    Vector3f SimpleRigidBody::GetLinearAcceleration() const
    {
        return m_acceleration;
    }

    void SimpleRigidBody::SetLinearAcceleration(const Vector3f& a)
    {
        m_acceleration = a;
    }

    bool SimpleRigidBody::IsDynamic() const
    {
        return m_type == BodyType::DYNAMIC;
    }

    void SimpleRigidBody::SetTotalMass(float m)
    {
        if (m <= 0)
        {
            m = 1.f;
        }

        float initialMass = m_mass;
        m_mass = m;

        // Set all collider masses relative to the change 
        float percentageChange = (m_mass - initialMass) / initialMass;
        for (SimpleCollider* col : m_colliders)
        {
            //col->m_mass += (col->m_mass * percentageChange);
            float mass = col->GetMass();
            col->AddMass(mass * percentageChange);
        }

        CalculateInverseInertiaTensor();
    }

    float SimpleRigidBody::GetTotalMass() const
    {
        return m_mass;
    }

    void SimpleRigidBody::AddMass(float m)
    {
        m_mass += m;
    }

    BodyType SimpleRigidBody::GetType() const
    {
        return m_type;
    }

    void SimpleRigidBody::SetAngularFreeAxis(const Vector3f& axis)
    {
        m_angularFreeAxis = axis;
    }

    void SimpleRigidBody::SetAngularDamping(float d)
    {
        m_angularDamping = d;
    }

    void SimpleRigidBody::CalculateInverseInertiaTensor()
    {
        if (m_mass <= 0.f)
        {
            return;
        }

        CalculateLocalCentreOfMass();

        Matrix3f I = Matrix3f(0.f);
        for (SimpleCollider* col : m_colliders)
        {
            Matrix3f Icentroid_local = col->CalculateInertiaTensor();

            //Rotate inertia into body frame
            Matrix3f Rc = ToMat3(col->GetLocalRotation());
            Matrix3f Icentroid_body = Rc * Icentroid_local * Transpose(Rc);

            //Parallel-axis from collider centroid to body COM
            Vector3f di = col->GetLocalPosition() - m_localCOM;
            float d2 = LengthSq(di);
            Matrix3f PA = col->GetMass() * (d2 * Identity3x3() - Outer(di, di));

            I += Icentroid_body + PA;
        }

        RecentreBodyToCOM(m_localCOM);

        m_inverseInertiaTensorLocal = Inverse(I);
    }

    Vector3f SimpleRigidBody::GetWorldCentreOfMass() const
    {
        Matrix3f Rb = ToMat3(m_rotation);
        return m_position - Rb * m_localCOM;
    }

    void SimpleRigidBody::CalculateLocalCentreOfMass()
    {
        m_localCOM = Vector3f(0.f);
        for (SimpleCollider* col : m_colliders)
        {
            m_localCOM += col->GetMass() * col->GetLocalPosition();
        }
        m_localCOM /= m_mass;
    }

    void SimpleRigidBody::RecentreBodyToCOM(const Vector3f& com)
    {
        Matrix3f Rb = ToMat3(m_rotation);
        Vector3f worldShift = Rb * com;

        m_position += worldShift;
        for (SimpleCollider* col : m_colliders)
        {
            //col->m_localPosition -= com;
            col->AdjustLocalPositionToCOM(com);
        }
    }

    std::vector<Collider*> SimpleRigidBody::GetColliders()
    {
        std::vector<Collider*> outColliders;

        for (SimpleCollider* col : m_colliders)
        {
            outColliders.push_back(static_cast<Collider*>(col));
        }

        return outColliders;
    }

    void SimpleRigidBody::AddCollider(SimpleCollider* col)
    {
        m_colliders.push_back(col);
    }

    AABB SimpleRigidBody::GetBroadphaseAABB() const
    {
        return m_broadphaseAABB;
    }

    void SimpleRigidBody::BroadphaseExpand(const AABB& other)
    {
        m_broadphaseAABB.Expand(other);
    }

    void SimpleRigidBody::BroadphaseReset()
    {
        m_broadphaseAABB.Reset();
    }

    Matrix3f SimpleRigidBody::GetInverseInertiaTensorWorld() const
    {
        return m_inverseInertiaTensorWorld;
    }

    void SimpleRigidBody::SetInverseInertiaTensorWorld(const Matrix3f& m)
    {
        m_inverseInertiaTensorWorld = m;
    }

    Matrix3f SimpleRigidBody::GetInverseInertiaTensorLocal() const
    {
        return m_inverseInertiaTensorLocal;
    }
}