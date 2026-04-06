#include "../scratch_rigidbody_collider.hpp"

namespace Physics::Backend
{
    BoxCollider::BoxCollider(RigidBody* body, float mass, const Vector3f& extents, const Vector3f& localPos, const Quaternion& localRot)
        : 
        SimpleCollider(body, localPos, localRot), 
        m_halfExtents(extents)
    {
        m_shapeType = ShapeType::BOX;

        if (m_parentBody->GetType() == BodyType::DYNAMIC)
        {
            m_oriented = true;
        }
        else
        {
            CheckStaticBoxAlignment();
        }

        SetMass(mass);
        RecomputeBroadPhaseAABB(0.2f);
    }

    void BoxCollider::RecomputeBroadPhaseAABB(float margin)
    {
        Matrix3f R = GetCompositeRotation();

        // abs(R): element-wise absolute value
        Matrix3f A;
        for (int i = 0; i < 3; ++i)
        {
            for (int j = 0; j < 3; ++j)
            {
                A[i][j] = std::abs(R[i][j]);
            }
        }

        if (m_oriented)
        {
            // world half-extents = abs(R) * localHalfExtents
            const Vector3f he = m_halfExtents;
            Vector3f e;
            e.x = A[0][0] * he.x + A[0][1] * he.y + A[0][2] * he.z;
            e.y = A[1][0] * he.x + A[1][1] * he.y + A[1][2] * he.z;
            e.z = A[2][0] * he.x + A[2][1] * he.y + A[2][2] * he.z;
            Vector3f broadphaseHalfExtents = e + Vector3f(margin);
            CalculateBroadphaseAABBThroughExtents(broadphaseHalfExtents);
        }
        else
        {
            // Axis-aligned in world -> broadphase extents are just the half extents (+ margin)
            Vector3f broadphaseHalfExtents = m_halfExtents + Vector3f(margin);
            CalculateBroadphaseAABBThroughExtents(broadphaseHalfExtents);
        }
    }

    bool BoxCollider::RayIntersect(const Ray& ray, float& outT)
    {
        Vector3f worldPos = GetWorldCentre();
        Vector3f min = worldPos - m_halfExtents;
        Vector3f max = worldPos + m_halfExtents;

        float tmin = (min.x - ray.origin.x) / ray.direction.x;
        float tmax = (max.x - ray.origin.x) / ray.direction.x;
        if (tmin > tmax)
        {
            std::swap(tmin, tmax);
        }

        float tymin = (min.y - ray.origin.y) / ray.direction.y;
        float tymax = (max.y - ray.origin.y) / ray.direction.y;
        if (tymin > tymax)
        {
            std::swap(tymin, tymax);
        }

        if ((tmin > tymax) || (tymin > tmax))
        {
            return false;
        }

        if (tymin > tmin)
        {
            tmin = tymin;
        }
        if (tymax < tmax)
        {
            tmax = tymax;
        }

        float tzmin = (min.z - ray.origin.z) / ray.direction.z;
        float tzmax = (max.z - ray.origin.z) / ray.direction.z;
        if (tzmin > tzmax)
        {
            std::swap(tzmin, tzmax);
        }

        if ((tmin > tzmax) || (tzmin > tmax))
        {
            return false;
        }

        if (tzmin > tmin)
        {
            tmin = tzmin;
        }
        if (tzmax < tmax)
        {
            tmax = tzmax;
        }

        outT = tmin;
        return outT >= 0;
    }

    Matrix3f BoxCollider::CalculateInertiaTensor()
    {
        float hx = m_halfExtents.x * 2.f;
        float hy = m_halfExtents.y * 2.f;
        float hz = m_halfExtents.z * 2.f;

        float x = (1.0f / 12.0f) * m_mass * (hy * hy + hz * hz);
        float y = (1.0f / 12.0f) * m_mass * (hx * hx + hz * hz);
        float z = (1.0f / 12.0f) * m_mass * (hx * hx + hy * hy);

        Matrix3f inertiaTensor = Diagonal3x3(Vector3f(x, y, z));
        return inertiaTensor;
    }

    void BoxCollider::CheckStaticBoxAlignment()
    {
        if (m_shapeType == ShapeType::BOX)
        {
            Matrix3f R = GetCompositeRotation();
            if (IsIdentity(R))
            {
                m_oriented = false;
            }
            else
            {
                m_oriented = true;
            }
        }
    }

    Vector3f BoxCollider::SupportPointOBB(const Vector3f& dir)
    {
        Matrix3f axes = GetCompositeRotation();
        Vector3f point = GetWorldCentre();
        point += axes[0] * ((Dot(axes[0], dir) >= 0.f) ? m_halfExtents.x : -m_halfExtents.x);
        point += axes[1] * ((Dot(axes[1], dir) >= 0.f) ? m_halfExtents.y : -m_halfExtents.y);
        point += axes[2] * ((Dot(axes[2], dir) >= 0.f) ? m_halfExtents.z : -m_halfExtents.z);
        return point;
    }

    void BoxCollider::GetOBBFace(int axis, int sign, // axis=0,1,2; sign=+1 or -1
        Vector3f& center, Vector3f& u, Vector3f& v,
        float& halfU, float& halfV,
        Vector3f verts[4])
    {
        const Matrix3f R = GetCompositeRotation();
        const Vector3f he = m_halfExtents;
        Vector3f Nx = R[0], Ny = R[1], Nz = R[2];

        Vector3f N = (axis == 0 ? Nx : (axis == 1 ? Ny : Nz)) * (float)sign;

        // choose u,v as the other two box axes (right-handed on the face)
        if (axis == 0) 
        { 
            u = Ny; 
            v = Nz; 
            halfU = he.y; 
            halfV = he.z; 
        }
        if (axis == 1) 
        { 
            u = Nz; 
            v = Nx; 
            halfU = he.z; 
            halfV = he.x; 
        }
        if (axis == 2) 
        { 
            u = Nx; 
            v = Ny; 
            halfU = he.x; 
            halfV = he.y; 
        }

        center = GetWorldCentre() + N * (axis == 0 ? he.x : (axis == 1 ? he.y : he.z));

        // corners in world
        verts[0] = center + u * halfU + v * halfV;
        verts[1] = center - u * halfU + v * halfV;
        verts[2] = center - u * halfU - v * halfV;
        verts[3] = center + u * halfU - v * halfV;
    }

    FaceRef BoxCollider::BuildReferenceFace(const Vector3f& n)
    {
        // choose axis of refBox most aligned with n
        const Matrix3f R = GetCompositeRotation();
        Vector3f axes[3] = { R[0], R[1], R[2] };
        float dots[3] = { fabsf(Dot(n, axes[0])), fabsf(Dot(n, axes[1])), fabsf(Dot(n, axes[2])) };
        int axis = (dots[0] > dots[1]) ? ((dots[0] > dots[2]) ? 0 : 2) : ((dots[1] > dots[2]) ? 1 : 2);
        int sign = (Dot(n, axes[axis]) > 0.f) ? +1 : -1;

        Vector3f center, u, v; float halfU, halfV; Vector3f tmp[4];
        GetOBBFace(axis, sign, center, u, v, halfU, halfV, tmp);

        FaceRef face;
        face.n = (sign > 0 ? axes[axis] : -axes[axis]);
        face.center = center;
        face.u = u; face.v = v; face.halfU = halfU; face.halfV = halfV;

        return face;
    }

    bool BoxCollider::IsOriented() const
    {
        return m_oriented;
    }

    Vector3f BoxCollider::GetHalfExtents() const
    {
        return m_halfExtents;
    }
}