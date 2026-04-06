#include "scratch_world.hpp"

namespace Physics 
{
    std::unique_ptr<World> CreatePhysicsWorld() 
    {
        return std::make_unique<Backend::SimpleWorld>();
    }
}

namespace Physics::Backend
{
    SimpleWorld::SimpleWorld()
        : 
        m_hasStarted(false), 
        m_gravity(Vector3f(0.f, -9.81f, 0.f))
    {

    }

    void SimpleWorld::SetGravity(const Vector3f& newGravity)
    {
        m_gravity = newGravity;
    }

    RigidBody* SimpleWorld::CreateRigidBody(const Vector3f& position, const Quaternion& rotation, BodyType type)
    {
        m_bodies.emplace_back(std::make_unique<SimpleRigidBody>(position, rotation, type));
        return m_bodies.back().get();

    }

    void SimpleWorld::RemoveRigidBody(RigidBody* ptr)
    {
        // Remove body
        auto it = std::remove_if(m_bodies.begin(), m_bodies.end(), [ptr](const std::unique_ptr<SimpleRigidBody>& b)
        {
                return b.get() == ptr;
        });

        if (it != m_bodies.end())
        {
            m_bodies.erase(it, m_bodies.end());
        }

        // Remove any colliders that belong to this body
        auto colIt = std::remove_if(m_colliders.begin(), m_colliders.end(), [ptr](const std::unique_ptr<SimpleCollider>& c)
        {
                return c && (c->GetParentBody() == ptr);
        });
        if (colIt != m_colliders.end())
        {
            m_colliders.erase(colIt, m_colliders.end());
        }
    }

    void SimpleWorld::Update(float dt)
    {
        if (!m_hasStarted)
        {
            for (const auto& bodyPtr : m_bodies)
            {
                SimpleRigidBody* body = bodyPtr.get();
                body->CalculateInverseInertiaTensor();
            }

            m_hasStarted = true;
        }

        for (const auto& bodyPtr : m_bodies)
        {
            SimpleRigidBody* body = bodyPtr.get(); // extract raw pointer safely
            IntegrateBody(body, dt);
        }

        RecomputeBroadphaseAABBs();

        m_contactManifolds.clear();

        for (int i = 0; i < m_bodies.size(); i++) //Compare unique rigidbodies
        {
            SimpleRigidBody* bodyA = m_bodies[i].get();

            for (int j = i + 1; j < m_bodies.size(); j++)
            {
                SimpleRigidBody* bodyB = m_bodies[j].get();

                if (bodyA->IsDynamic() || bodyB->IsDynamic()) //Don't detect collision if both are static
                {
                    if (BroadphaseCollisionDetection(bodyA->GetBroadphaseAABB(), bodyB->GetBroadphaseAABB()))
                    {
                        for (Collider* colliderA : bodyA->GetColliders()) //Detect broadphase collision between the two rigidbodies' colliders
                        {
                            for (Collider* colliderB : bodyB->GetColliders())
                            {
                                SimpleCollider* colA = static_cast<SimpleCollider*>(colliderA);
                                SimpleCollider* colB = static_cast<SimpleCollider*>(colliderB);

                                if (BroadphaseCollisionDetection(colA->GetBroadphaseAABB(), colB->GetBroadphaseAABB()))
                                {
                                    NarrowphaseCollisionDetection(colA, colB);
                                }
                            }
                        }
                    }
                }
            }
        }

        for (const ContactManifold& m : m_contactManifolds)
        {
            ResolveCollision(m);
        }

        for (CollisionCallback* cb : m_collisionCallBacks)
        {
            cb->Update(m_contactManifolds);
        }
    }

    RaycastHit SimpleWorld::Raycast(const Ray& ray, float maxDistance)
    {
        RaycastHit closestHit;

        for (const auto& colliderPtr : m_colliders)
        {
            SimpleCollider* col = colliderPtr.get();

            float dist;
            bool hit = false;

            Vector3f worldPos = col->GetWorldCentre();
            hit = col->RayIntersect(ray, dist);

            if (hit && dist < closestHit.distance && dist <= maxDistance) 
            {
                closestHit.hit = true;
                closestHit.distance = dist;
                closestHit.point = ray.origin + ray.direction * dist;

                closestHit.collider = col;
                closestHit.body = col->GetParentBody();
            }
        }

        return closestHit;
    }

    //REVIEW THESE ADD COLLIDER FUNCTIONS
    Collider* SimpleWorld::AddBoxCollider(RigidBody* body, const AABB& aabb, float mass, const Vector3f& localPosition, const Quaternion& localRotation)
    {
        Vector3f size = aabb.max - aabb.min;
        Vector3f halfExtents = size * 0.5f;

        // Implicit upcast unique_ptr<BoxCollider> -> unique_ptr<SimpleCollider> is allowed
        m_colliders.emplace_back(std::make_unique<BoxCollider>(body, mass, halfExtents, localPosition, localRotation));

        return m_colliders.back().get();
    }



    Collider* SimpleWorld::AddSphereCollider(RigidBody* body, float radius, float mass, const Vector3f& localPosition, const Quaternion& localRotation)
    {
        // Implicit upcast unique_ptr<SphereCollider> -> unique_ptr<SimpleCollider> is allowed
        m_colliders.emplace_back(std::make_unique<SphereCollider>(body, mass, radius, localPosition, localRotation));

        return m_colliders.back().get();
    }

    Collider* SimpleWorld::AddSphereCollider(RigidBody* body, const AABB& aabb, float mass, const Vector3f& localPosition, const Quaternion& localRotation)
    {
        Vector3f size = aabb.max - aabb.min;
        float radius = glm::length(size) * 0.5f;

        // Implicit upcast unique_ptr<SphereCollider> -> unique_ptr<SimpleCollider> is allowed
        m_colliders.emplace_back(std::make_unique<SphereCollider>(body, mass, radius, localPosition, localRotation));

        return m_colliders.back().get();
    }




    Collider* SimpleWorld::AddCapsuleCollider(RigidBody* body, float radius, float height, float mass, const Vector3f& localPosition)
    {
        // Implicit upcast unique_ptr<CapsuleCollider> -> unique_ptr<SimpleCollider> is allowed
        m_colliders.emplace_back(std::make_unique<CapsuleCollider>(body, localPosition, mass, radius, height));

        return m_colliders.back().get();
    }

    //Collider* SimpleWorld::AddCapsuleCollider(RigidBody*, const AABB&, float mass, const Vector3f& localPosition = Vector3f(0.f)) { return new SimpleCollider(); }

    void SimpleWorld::RegisterCollisionCallback(CollisionCallback* cb)
    {
        if (cb)
        {
            bool IsDuplicate = false;

            for (CollisionCallback* it : m_collisionCallBacks)
            {
                if (it == cb)
                {
                    IsDuplicate = true;
                }
            }

            if (!IsDuplicate)
            {
                m_collisionCallBacks.push_back(cb);
            }
        }
    }

    void SimpleWorld::UnregisterCollisionCallback(CollisionCallback* cb)
    {
        if (cb)
        {
            auto it = std::remove(m_collisionCallBacks.begin(), m_collisionCallBacks.end(), cb);
            m_collisionCallBacks.erase(it, m_collisionCallBacks.end());
        }
    }

    void SimpleWorld::PrintVector(std::string variable, const Vector3f& v)
    {
        std::cout << variable << ": " << v.x << ", " << v.y << ", " << v.z << "\n";
    }

    void SimpleWorld::IntegrateBody(SimpleRigidBody* body, float dt)
    {
        if (body->GetType() == BodyType::DYNAMIC)
        {
            // === Linear integration ===
            if (body->IsGravityEnabled())
            {
                Vector3f netForce = body->GetNetForce();
                body->SetNetForce(netForce + body->GetTotalMass() * m_gravity);
            }

            Vector3f acceleration = body->GetLinearAcceleration();
            body->SetLinearAcceleration(acceleration + body->GetNetForce() / body->GetTotalMass());
            Vector3f velocity = body->GetLinearVelocity();
            body->SetLinearVelocity(velocity + body->GetLinearAcceleration() * dt);
            Vector3f position = body->GetPosition();
            body->SetPosition(position + body->GetLinearVelocity() * dt);

            body->SetLinearAcceleration(Vector3f(0.f));
            body->SetNetForce(Vector3f(0.f));

            // === Angular Integration ===
            Vector3f w = body->GetAngularVelocity();
            float wLength = Length(w);

            Quaternion qDelta;
            if (wLength > 1e-8f)
            {
                float theta = wLength * dt;
                Vector3f axis = w / wLength;
                qDelta = Quaternion(cos(theta / 2), axis * sin(theta / 2));
            }
            else
            {
                Vector3f half = 0.5f * w * dt;
                qDelta = Quaternion(1.0f, half.x, half.y, half.z);
            }

            Quaternion rotation = body->GetOrientation();
            body->SetOrientation(Normalize(qDelta * rotation));

            Matrix3f R = ToMat3(body->GetOrientation());
            body->SetInverseInertiaTensorWorld(R * body->GetInverseInertiaTensorLocal() * Transpose(R));
        }
    }

    void SimpleWorld::RecomputeBroadphaseAABBs()
    {
        const float margin = 0.3f;

        for (const auto& bodyPtr : m_bodies)
        {
            SimpleRigidBody* body = bodyPtr.get();
            if (body->IsDynamic())
            {
                body->BroadphaseReset();

                for (Collider* collider : body->GetColliders())
                {
                    SimpleCollider* col = static_cast<SimpleCollider*>(collider);
                    col->RecomputeBroadPhaseAABB(margin);
                }
            }
        }
    }

    bool SimpleWorld::BroadphaseCollisionDetection(const AABB& a, const AABB& b)
    {
        // Check for overlap on all axes
        bool overlapX = a.max.x >= b.min.x && a.min.x <= b.max.x;
        bool overlapY = a.max.y >= b.min.y && a.min.y <= b.max.y;
        bool overlapZ = a.max.z >= b.min.z && a.min.z <= b.max.z;

        return (overlapX && overlapY && overlapZ);
    }

    void SimpleWorld::NarrowphaseCollisionDetection(SimpleCollider* colA, SimpleCollider* colB)
    {
        switch (colA->GetShapeType())
        {
        case ShapeType::SPHERE:
        {
            SphereCollider* sphere = static_cast<SphereCollider*>(colA);
            switch (colB->GetShapeType())
            {
            case ShapeType::SPHERE:
            {
                SphereCollider* otherSphere = static_cast<SphereCollider*>(colB);
                SphereToSphereCollision(sphere, otherSphere);
            } break;
            case ShapeType::BOX:
            {
                BoxCollider* box = static_cast<BoxCollider*>(colB);
                BoxToSphereCollision(box, sphere);
            } break;
            }
        } break;

        case ShapeType::BOX:
        {
            BoxCollider* box = static_cast<BoxCollider*>(colA);
            switch (colB->GetShapeType())
            {
            case ShapeType::SPHERE:
            {
                SphereCollider* sphere = static_cast<SphereCollider*>(colB);
                BoxToSphereCollision(box, sphere);
            } break;
            case ShapeType::BOX:
            {
                BoxCollider* otherBox = static_cast<BoxCollider*>(colB);
                BoxToBoxCollision(box, otherBox);
            } break;
            }
        } break;
        }
    }

    void SimpleWorld::BoxToBoxCollision(BoxCollider* colA, BoxCollider* colB)
    {
        if (!colA->IsOriented() && !colB->IsOriented())
        {
            AABBToAABBCollision(colA, colB);
        }
        else
        {
            OBBToOBBCollision(colA, colB);
        }
    }

    void SimpleWorld::BoxToSphereCollision(BoxCollider* colBox, SphereCollider* colSphere)
    {
        if (colBox->IsOriented())
        {
            OBBToSphereCollision(colBox, colSphere);
        }
        else
        {
            AABBToSphereCollision(colBox, colSphere);
        }
    }
    /*
    void SimpleWorld::BoxToCapsuleCollision(BoxCollider* boxCol, CapsuleCollider* capsuleCol)
    {
        if (boxCol->oriented)
        {
            OBBToCapsuleCollision(boxCol, capsuleCol);
        }
        else
        {
            AABBToCapsuleCollision(boxCol, capsuleCol);
        }
    }
    */
    void SimpleWorld::SphereToSphereCollision(SphereCollider* colA, SphereCollider* colB)
    {
        ContactManifold m;

        Vector3f posA = colA->GetWorldCentre();
        float radiusA = colA->GetRadius();

        Vector3f posB = colB->GetWorldCentre();
        float radiusB = colB->GetRadius();

        Vector3f delta = posB - posA;
        float distSq = Dot(delta, delta);
        float rSum = radiusA + radiusB;

        if (distSq <= rSum * rSum) 
        {
            m.hit = true;
            m.colA = colA;
            m.colB = colB;

            float dist = sqrtf(distSq);
            Vector3f normal = (dist > 1e-4f) ? (delta / dist) : Vector3f(1, 0, 0);
            float penetration = rSum - dist;

            // Midpoint between centers is good enough for now
            Vector3f p = posA + 0.5f * (posB - posA);

            // (If you have body pointers, use their positions here)
            AddContact(m, p, normal, penetration);

            m_contactManifolds.push_back(m);
        }
    }

    void SimpleWorld::AABBToAABBCollision(BoxCollider* colA, BoxCollider* colB)
    {
        ContactManifold m;

        Vector3f posA = colA->GetWorldCentre();
        Vector3f minA = posA - colA->GetHalfExtents();
        Vector3f maxA = posA + colA->GetHalfExtents();

        Vector3f posB = colB->GetWorldCentre();
        Vector3f minB = posB - colB->GetHalfExtents();
        Vector3f maxB = posB + colB->GetHalfExtents();

        bool overlapX = maxA.x >= minB.x && minA.x <= maxB.x;
        bool overlapY = maxA.y >= minB.y && minA.y <= maxB.y;
        bool overlapZ = maxA.z >= minB.z && minA.z <= maxB.z;

        if (overlapX && overlapY && overlapZ) 
        {
            m.hit = true;
            m.colA = colA;
            m.colB = colB;

            float penX = std::min(maxA.x - minB.x, maxB.x - minA.x);
            float penY = std::min(maxA.y - minB.y, maxB.y - minA.y);
            float penZ = std::min(maxA.z - minB.z, maxB.z - minA.z);

            Vector3f normal;
            // Minimum translation axis + sign
            if (penX < penY && penX < penZ) 
            {
                normal = Vector3f((posB.x < posA.x) ? -1.f : 1.f, 0.f, 0.f);
            }
            else if (penY < penZ) 
            {
                normal = Vector3f(0.f, (posB.y < posA.y) ? -1.f : 1.f, 0.f);
            }
            else 
            {
                normal = Vector3f(0.f, 0.f, (posB.z < posA.z) ? -1.f : 1.f);
            }

            // Contact point: center of overlap region (still a single point)
            Vector3f contactMin = glm::max(minA, minB);
            Vector3f contactMax = glm::min(maxA, maxB);
            Vector3f p = (contactMin + contactMax) * 0.5f;

            float penetration = (normal.x != 0.f) ? penX : (normal.y != 0.f) ? penY : penZ;

            AddContact(m, p, normal, penetration);
            m_contactManifolds.push_back(m);
        }
    }

    void SimpleWorld::AABBToSphereCollision(BoxCollider* colBox, SphereCollider* colSphere)
    {
        ContactManifold m;

        Vector3f boxPos = colBox->GetWorldCentre();
        Vector3f spherePos = colSphere->GetWorldCentre();

        Vector3f boxMin = boxPos - colBox->GetHalfExtents();
        Vector3f boxMax = boxPos + colBox->GetHalfExtents();

        Vector3f closest;
        closest.x = Clamp(spherePos.x, boxMin.x, boxMax.x);
        closest.y = Clamp(spherePos.y, boxMin.y, boxMax.y);
        closest.z = Clamp(spherePos.z, boxMin.z, boxMax.z);

        Vector3f delta = spherePos - closest;
        float distSq = Dot(delta, delta);
        float r = colSphere->GetRadius();

        if (distSq <= r * r) 
        {
            m.hit = true;
            m.colA = colBox;
            m.colB = colSphere;

            Vector3f normal;
            float dist = (distSq > 1e-8f) ? sqrtf(distSq) : 0.f;
            if (dist > 1e-4f) 
            {
                normal = delta / dist;
            }
            else 
            {
                // Guess an axis from box center
                Vector3f boxCenter = (boxMin + boxMax) * 0.5f;
                Vector3f guess = spherePos - boxCenter;
                if (fabsf(guess.x) > fabsf(guess.y) && fabsf(guess.x) > fabsf(guess.z))
                {
                    normal = Vector3f((guess.x >= 0.f) ? 1.f : -1.f, 0.f, 0.f);
                }
                else if (fabsf(guess.y) > fabsf(guess.z))
                {
                    normal = Vector3f(0.f, (guess.y >= 0.f) ? 1.f : -1.f, 0.f);
                }
                else
                {
                    normal = Vector3f(0.f, 0.f, (guess.z >= 0.f) ? 1.f : -1.f);
                }
            }

            float penetration = r - dist;
            AddContact(m, closest, normal, penetration);
            m_contactManifolds.push_back(m);
        }
    }

    void SimpleWorld::OBBToSphereCollision(BoxCollider* boxCol, SphereCollider* sphereCol)
    {
        ContactManifold m;

        Vector3f boxCentre = boxCol->GetWorldCentre();
        Matrix3f boxAxes = boxCol->GetCompositeRotation();

        Vector3f sphereCentre = sphereCol->GetWorldCentre();
        float r = sphereCol->GetRadius();

        Vector3f d = sphereCentre - boxCentre;
        Vector3f local;
        local.x = Dot(d, boxAxes[0]);
        local.y = Dot(d, boxAxes[1]);
        local.z = Dot(d, boxAxes[2]);

        Vector3f boxExtents = boxCol->GetHalfExtents();

        Vector3f qLocal;
        qLocal.x = Clamp(local.x, -boxExtents.x, boxExtents.x);
        qLocal.y = Clamp(local.y, -boxExtents.y, boxExtents.y);
        qLocal.z = Clamp(local.z, -boxExtents.z, boxExtents.z);

        Vector3f Q = boxCentre + boxAxes[0] * qLocal.x + boxAxes[1] * qLocal.y + boxAxes[2] * qLocal.z;

        Vector3f delta = sphereCentre - Q;
        float distSq = Dot(delta, delta);

        if (distSq <= r * r) 
        {
            m.hit = true;
            m.colA = boxCol;
            m.colB = sphereCol;

            Vector3f normal;
            float dist = (distSq > 1e-8f) ? sqrtf(distSq) : 0.f;
            if (dist > 1e-4f)
            {
                normal = delta / dist;
            }
            else
            {
                // choose major axis of penetration
                Vector3f rel = local;
                Vector3f gap(fabsf(rel.x) - boxExtents.x,
                    fabsf(rel.y) - boxExtents.y,
                    fabsf(rel.z) - boxExtents.z);
                if (fabsf(gap.x) >= fabsf(gap.y) && fabsf(gap.x) >= fabsf(gap.z))
                {
                    normal = (rel.x >= 0.f) ? boxAxes[0] : -boxAxes[0];
                }
                else if (fabsf(gap.y) >= fabsf(gap.z))
                {
                    normal = (rel.y >= 0.f) ? boxAxes[1] : -boxAxes[1];
                }
                else
                {
                    normal = (rel.z >= 0.f) ? boxAxes[2] : -boxAxes[2];
                }
            }

            float penetration = r - ((dist > 0.f) ? dist : 0.f);
            AddContact(m, Q, normal, penetration);
            m_contactManifolds.push_back(m);
        }
    }

    void SimpleWorld::OBBToOBBCollision(BoxCollider* A, BoxCollider* B)
    {
        Vector3f n; float pen;
        if (!OBB_SAT(A, B, n, pen))
        {
            return;
        }

        ContactManifold m;
        m.hit = true; 
        m.colA = A; 
        m.colB = B;

        // make n point A->B
        Vector3f cA = A->GetWorldCentre(), cB = B->GetWorldCentre();
        Vector3f normal = (Dot(n, cB - cA) >= 0.f) ? n : -n;

        // Decide reference vs incident
        // Heuristic: the box whose face is most aligned with n is reference
        FaceRef ref = A->BuildReferenceFace(normal);
        // Also build alignment score for B; if B is more aligned, swap
        FaceRef refB = B->BuildReferenceFace(-normal);
        float scoreA = fabsf(Dot(normal, ref.n));
        float scoreB = fabsf(Dot(normal, -refB.n));
        BoxCollider* refBox = A;
        BoxCollider* incBox = B;
        FaceRef chosen = ref;
        if (scoreB > scoreA) 
        { 
            refBox = B; 
            incBox = A; 
            chosen = refB; 
            normal = -normal; 
        }

        // Get incident face on incBox (most anti-parallel to m.normal)
        const Matrix3f Rinc = incBox->GetCompositeRotation();
        Vector3f iAxes[3] = { Rinc[0], Rinc[1], Rinc[2] };
        float bestDot = 1e9f; int incAxis = 0; int incSign = +1;
        for (int k = 0; k < 3; k++) 
        {
            float dPlus = Dot(iAxes[k], normal);
            float dMinus = Dot(-iAxes[k], normal);
            if (dPlus < bestDot) 
            { 
                bestDot = dPlus;  
                incAxis = k; 
                incSign = +1; 
            }
            if (dMinus < bestDot) 
            { 
                bestDot = dMinus; 
                incAxis = k; 
                incSign = -1; 
            }
        }
        Vector3f incC, iu, iv; float ihU, ihV; Vector3f incVerts[4];
        incBox->GetOBBFace(incAxis, incSign, incC, iu, iv, ihU, ihV, incVerts);

        // Clip incident quad against the 4 side planes of the reference face
        // Reference face side planes: ±u at halfU, ±v at halfV
        Vector3f polyA[8], polyB[8];
        int count = 4;
        memcpy(polyA, incVerts, sizeof(Vector3f) * 4);

        auto clip = [&](const Vector3f& pn, float pd, Vector3f* inPts, int inN) 
        {
            return ClipPolygonAgainstPlane(pn, pd, inPts, inN, polyB);
        };

        // Plane format: pn·x <= d
        // Side +u
        count = clip(chosen.u, Dot(chosen.u, chosen.center) + chosen.halfU, polyA, count);
        memcpy(polyA, polyB, sizeof(Vector3f) * count);
        // Side -u
        count = clip(-chosen.u, -Dot(chosen.u, chosen.center) + chosen.halfU, polyA, count);
        memcpy(polyA, polyB, sizeof(Vector3f) * count);
        // Side +v
        count = clip(chosen.v, Dot(chosen.v, chosen.center) + chosen.halfV, polyA, count);
        memcpy(polyA, polyB, sizeof(Vector3f) * count);
        // Side -v
        count = clip(-chosen.v, -Dot(chosen.v, chosen.center) + chosen.halfV, polyA, count);
        memcpy(polyA, polyB, sizeof(Vector3f) * count);

        if (count == 0)
        {
            return; // robust guard
        }

        // For each clipped point, compute penetration along normal and add
        // depth = (ref face plane) distance - projection
        float refPlaneD = Dot(normal, chosen.center);
        for (int i = 0; i < count; i++)
        {
            const Vector3f& v = polyB[i];
            float distAlongN = refPlaneD - Dot(normal, v); // positive if penetrating
            if (distAlongN >= -1e-4f)  // small tolerance
            {
                float depth = std::max(0.f, distAlongN);
                AddContact(m, v, normal, depth);
            }
        }

        if (m.count == 0)
        {
            // fallback: use support points midpoint (your current approach)
            Vector3f pA = A->SupportPointOBB(normal);
            Vector3f pB = B->SupportPointOBB(-normal);
            float depth = pen;
            AddContact(m, 0.5f * (pA + pB), normal, depth);
        }

        m_contactManifolds.push_back(m);
    }
    
    void SimpleWorld::AddContact(ContactManifold& M, const Vector3f& p, const Vector3f& n, float pen)
    {
        Vector3f nUnit = Normalize(n);

        // consider it a duplicate only if the point is the same *and* normals agree
        for (int i = 0; i < M.count; ++i) 
        {
            if (AlmostSamePoint(M.points[i].position, p)) 
            {
                if (Dot(M.points[i].normal, nUnit) > 0.95f) 
                {
                    return; // same constraint
                }
                // else: same point but different normal -> keep both
            }
        }

        if (M.count == 4) 
        {
            int shallow = 0;
            for (int i = 1; i < 4; ++i)
            {
                if (M.points[i].penetration < M.points[shallow].penetration)
                {
                    shallow = i;
                }
            }
            if (pen > M.points[shallow].penetration) 
            {
                M.points[shallow].position = p;
                M.points[shallow].normal = nUnit;
                M.points[shallow].penetration = pen;
            }
            return;
        }

        ContactPoint& cp = M.points[M.count++];
        cp.position = p;
        cp.normal = nUnit;
        cp.penetration = pen;
    }

    bool SimpleWorld::AlmostSamePoint(const Vector3f& a, const Vector3f& b, float tol)
    {
        return LengthSq(a - b) < tol * tol;
    }

    int SimpleWorld::ClipPolygonAgainstPlane(const Vector3f& pn, float pd, const Vector3f* inPts, int inCount, Vector3f* outPts)
    {
        int outN = 0;
        for (int i = 0; i < inCount; i++) 
        {
            Vector3f A = inPts[i];
            Vector3f B = inPts[(i + 1) % inCount];
            float da = Dot(pn, A) - pd;
            float db = Dot(pn, B) - pd;
            bool Ain = da <= 0.f;
            bool Bin = db <= 0.f;
            if (Ain && Bin) 
            { 
                outPts[outN++] = B; 
            }
            else if (Ain && !Bin) 
            {
                float t = da / (da - db);
                outPts[outN++] = A + t * (B - A);
            }
            else if (!Ain && Bin) 
            {
                float t = da / (da - db);
                outPts[outN++] = A + t * (B - A);
                outPts[outN++] = B;
            }
        }
        return outN;
    }

    void SimpleWorld::ResolveCollision(const ContactManifold& m)
    {
        SimpleRigidBody* A = static_cast<SimpleRigidBody*>(m.colA->GetParentBody());
        SimpleRigidBody* B = static_cast<SimpleRigidBody*>(m.colB->GetParentBody());

        // Inverse masses (0 for static/kinematic)
        const float invMassA = A->IsDynamic() ? (1.0f / A->GetTotalMass()) : 0.0f;
        const float invMassB = B->IsDynamic() ? (1.0f / B->GetTotalMass()) : 0.0f;

        // Materials
        float restitution = std::min(m.colA->GetBounciness(), m.colB->GetBounciness());
        const float mu = std::sqrt(m.colA->GetFriction() * m.colB->GetFriction());

        for (int i = 0; i < m.count; ++i)
        {
            const Vector3f& p = m.points[i].position;
            const float pen = m.points[i].penetration;

            // Make sure normal points A -> B and is normalized
            Vector3f n = m.points[i].normal;
            n = Normalize(n);
            if (Dot(n, (m.colB->GetWorldCentre() - m.colA->GetWorldCentre())) < 0.0f)
            {
                n = -n;
            }

            // === Normal impulse === //
            Vector3f rA = p - A->GetPosition();
            Vector3f rB = p - B->GetPosition();

            Vector3f rnA = Cross(rA, n);
            Vector3f rnB = Cross(rB, n);

            // Relative velocity at the contact point
            float linearVelocityAlongNormal = Dot(n, B->GetLinearVelocity() - A->GetLinearVelocity());
            float angularVelocityAlongNormal = Dot(B->GetAngularVelocity(), rnB) - Dot(A->GetAngularVelocity(), rnA);

            //Relative normal speed (velocity along the contact normal)
            float vN = linearVelocityAlongNormal + angularVelocityAlongNormal;
            float normalImpulseMag = 0.f;

            float angularFactorA = Dot(rnA, A->GetInverseInertiaTensorWorld() * rnA);
            float angularFactorB = Dot(rnB, B->GetInverseInertiaTensorWorld() * rnB);

            // Directional effective mass along the contact normal
            const float kN = invMassA + invMassB + (angularFactorA + angularFactorB);

            if (vN < 0.f)
            {
                normalImpulseMag = -((1 + restitution) * vN) / kN;
                Vector3f impulseN = normalImpulseMag * n;

                // Apply normal impulse  
                ApplyLinearImpulseToBody(A, -impulseN);
                ApplyLinearImpulseToBody(B, +impulseN);

                ApplyAngularImpulseToBody(A, -impulseN, p);
                ApplyAngularImpulseToBody(B, +impulseN, p);

                // === Friction impulse (Coulomb) === //
                Vector3f vRelAfter = (B->GetLinearVelocity() + Cross(B->GetAngularVelocity(), rB)) - (A->GetLinearVelocity() + Cross(A->GetAngularVelocity(), rA)); // Recompute relative velocity after normal impulse
                Vector3f vT = vRelAfter - Dot(n, vRelAfter) * n; //Tangential velocity

                float vTLength = Length(vT);
                if (vTLength > 0)
                {
                    //Unit tangent
                    Vector3f t = vT / vTLength;

                    Vector3f angularFactorA = Cross(A->GetInverseInertiaTensorWorld() * Cross(rA, t), rA);
                    Vector3f angularFactorB = Cross(B->GetInverseInertiaTensorWorld() * Cross(rB, t), rB);
                    //Directional effective mass along the unit tangent
                    float denominator = invMassA + invMassB + Dot(t, angularFactorA + angularFactorB);
                    //Tangential impulse magnitude
                    float impulseMag = -(Dot(t, vRelAfter) / denominator);
                    impulseMag = Clamp(impulseMag, -mu * Absf(normalImpulseMag), mu * Absf(normalImpulseMag)); //Coulomb clamp
                    Vector3f impulseT = impulseMag * t;

                    // Apply tangential impulse
                    ApplyLinearImpulseToBody(A, -impulseT);
                    ApplyLinearImpulseToBody(B, +impulseT);

                    ApplyAngularImpulseToBody(A, -impulseT, p);
                    ApplyAngularImpulseToBody(B, +impulseT, p);
                }
            }

            // === Positional correction === //
            const float kPercent = 0.2f;   // solve 20% of error per step
            float corr = pen * kPercent;

            float kLinear = invMassA + invMassB;

            if (corr > 0.f)
            {
                Vector3f correction = (corr / kLinear) * n;

                if (A->IsDynamic())
                {
                    Vector3f positionDelta = invMassA * correction;
                    Vector3f initialPos = A->GetPosition();
                    A->SetPosition(initialPos - positionDelta);
                }
                if (B->IsDynamic())
                {
                    Vector3f positionDelta = invMassB * correction;
                    Vector3f initialPos = B->GetPosition();
                    B->SetPosition(initialPos + positionDelta);
                }
            }
        }
    }

    void SimpleWorld::ApplyLinearImpulseToBody(Physics::RigidBody* body, const Vector3f& impulse)
    {
        if (body->GetType() == BodyType::DYNAMIC)
        {
            Vector3f vDelta = impulse / body->GetTotalMass();
            Vector3f vCurrent = body->GetLinearVelocity();
            body->SetLinearVelocity(vCurrent + vDelta);
        }
    }

    void SimpleWorld::ApplyAngularImpulseToBody(Physics::RigidBody* body, const Vector3f& impulse, const Vector3f& point)
    {
        SimpleRigidBody* rb = static_cast<SimpleRigidBody*>(body);
        if (rb->GetType() == BodyType::DYNAMIC)
        {
            Vector3f r = point - rb->GetPosition();
            Vector3f wDelta = rb->GetInverseInertiaTensorWorld() * Cross(r, impulse);
            Vector3f wCurrent = rb->GetAngularVelocity();
            rb->SetAngularVelocity(wCurrent + wDelta);
        }
    }

    Vector3f SimpleWorld::ClosestPointOnSegment(const Vector3f& a, const Vector3f& b, const Vector3f& p)
    {
        Vector3f ab = b - a;
        float t = Dot(p - a, ab) / Dot(ab, ab);
        t = Clamp(t, 0.0f, 1.0f);
        return a + ab * t;
    }

    // SAT core between two OBBs (A,B). Returns true if overlap. Also outputs MTV normal (world, unit-ish) and penetration depth.
    inline bool SimpleWorld::OBB_SAT(BoxCollider* boxA, BoxCollider* boxB, Vector3f& outNormal, float& outPen)
    {
        Vector3f centreA = boxA->GetWorldCentre();
        Vector3f centreB = boxB->GetWorldCentre();

        Matrix3f axesA = boxA->GetCompositeRotation();
        Matrix3f axesB = boxB->GetCompositeRotation();

        Vector3f extentsA = boxA->GetHalfExtents();
        Vector3f extentsB = boxB->GetHalfExtents();

        // Based on RTCD (Ericson) and OBBTree’s classic test.
        // Compute rotation matrix expressing B in A’s basis: R[i][j] = Ai dot Bj
        float EPS = 1e-5f;

        float Rm[3][3], AbsR[3][3];
        for (int i = 0; i < 3; ++i) 
        {
            const Vector3f& Ai = axesA[i];
            for (int j = 0; j < 3; ++j) 
            {
                const Vector3f& Bj = axesB[j];
                Rm[i][j] = Dot(Ai, Bj);
                AbsR[i][j] = std::abs(Rm[i][j]) + EPS; // epsilon to counter FP errors
            }
        }

        // t = B.c - A.c in A’s basis
        Vector3f tWorld = centreB - centreA;
        Vector3f t = { Dot(tWorld, axesA[0]), Dot(tWorld, axesA[1]), Dot(tWorld, axesA[2]) };

        // Track min penetration (MTV)
        float minPen = std::numeric_limits<float>::infinity();
        Vector3f bestAxis = Vector3f(0.f);

        auto test_axis = [&](const Vector3f& axisWorld, float dist, float ra, float rb) -> bool 
        {
            // dist is |t projected on axis| already (non-negative)
            float overlap = (ra + rb) - dist;
            if (overlap < 0.f)
            {
                return false;
            }
            if (overlap < minPen) 
            {
                minPen = overlap;
                // Direction goes from A to B, so normal sign should push A out of B
                // We use axisWorld normalized-ish; ensure direction agrees with (B.c - A.c)
                float sign = (Dot(axisWorld, (centreB - centreA)) < 0.f) ? -1.f : 1.f;
                bestAxis = axisWorld * sign;
            }
            return true;
        };

        // 1) Test A’s face axes: A0,A1,A2
        for (int i = 0; i < 3; ++i) 
        {
            float ra = extentsA[i];
            float rb = extentsB.x * AbsR[i][0] + extentsB.y * AbsR[i][1] + extentsB.z * AbsR[i][2];
            float dist = std::abs(t[i]);
            if (!test_axis(axesA[i], dist, ra, rb))
            {
                return false;
            }
        }

        // 2) Test B’s face axes: B0,B1,B2
        for (int j = 0; j < 3; ++j) 
        {
            float ra = extentsA.x * AbsR[0][j] + extentsA.y * AbsR[1][j] + extentsA.z * AbsR[2][j];
            float tb = Dot(tWorld, axesB[j]); // projection of t on Bj directly
            float dist = std::abs(tb);
            float rb = extentsB[j];
            if (!test_axis(axesB[j], dist, ra, rb))
            {
                return false;
            }
        }

        // 3) Test 9 edge cross axes: Ai x Bj
        for (int i = 0; i < 3; ++i)
        {
            for (int j = 0; j < 3; ++j)
            {
                // Cross axis in world
                Vector3f axis = Cross(axesA[i], axesB[j]);
                float len2 = LengthSq(axis);
                if (len2 < 1e-8f)
                {
                    continue; // nearly parallel; skip
                }
                axis = axis / glm::sqrt(len2);

                // Distance: |t dot axis|
                float dist = std::abs(Dot(tWorld, axis));

                // Radii:
                // ra = sum over Ak: A.e[k]*|(Ak dot axis)|
                float ra = extentsA.x * std::abs(Dot(axesA[0], axis))
                    + extentsA.y * std::abs(Dot(axesA[1], axis))
                    + extentsA.z * std::abs(Dot(axesA[2], axis));

                float rb = extentsB.x * std::abs(Dot(axesB[0], axis))
                    + extentsB.y * std::abs(Dot(axesB[1], axis))
                    + extentsB.z * std::abs(Dot(axesB[2], axis));

                if (!test_axis(axis, dist, ra, rb))
                {
                    return false;
                }
            }
        }

        outNormal = Normalize(bestAxis);
        outPen = minPen;
        return true;
    }
}