#ifndef SCRATCH_WORLD_HPP
#define SCRATCH_WORLD_HPP

#include "scratch_rigidbody_collider.hpp"

#include <algorithm>
#include <unordered_map>
#include <iostream>

namespace Physics::Backend
{
    /**
     * @class SimpleWorld
     * @brief Physics backend implementing @ref Physics::World.
     *
     * Owns bodies and colliders, steps simulation, runs broadphase/narrowphase,
     * resolves contacts, and supports basic raycasting and callbacks.
     */
    class SimpleWorld : public World
    {
    public:
        /**
         * @brief Construct an empty world with default settings.
         */
        SimpleWorld();

        /**
         * @brief Virtual destructor.
         */
        ~SimpleWorld() override = default;

        /**
         * @brief Set global gravity.
         * @param newGravity Gravity vector (m/s^2).
         */
        void SetGravity(const Vector3f& newGravity) override;

        /**
         * @brief Create a rigid body.
         * @param position Initial world position.
         * @param rotation Initial world rotation.
         * @param type Body type (DYNAMIC/KINEMATIC).
         * @return Newly created body owned by the world.
         */
        RigidBody* CreateRigidBody(const Vector3f& position, const Quaternion& rotation, BodyType type) override;

        /**
         * @brief Remove and destroy a rigid body.
         * @param ptr Body to remove.
         */
        void RemoveRigidBody(RigidBody* ptr) override;

        /**
         * @brief Advance the simulation by a time step.
         * @param dt Delta time in seconds.
         */
        void Update(float dt) override;

        /**
         * @brief Cast a ray against all colliders.
         * @param ray Ray origin/direction (direction normalized).
         * @param maxDistance Maximum distance (meters).
         * @return Closest hit, or hit=false if none.
         */
        RaycastHit Raycast(const Ray& ray, float maxDistance = std::numeric_limits<float>::max()) override;

        /**
         * @brief Add a box collider to a body.
         * @param body Parent body.
         * @param aabb Local-space AABB describing size.
         * @param mass Mass contribution (kg).
         * @param localPosition Local offset.
         * @param localRotation Local rotation.
         * @return New collider owned by the world/body.
         */
        Collider* AddBoxCollider(RigidBody* body, const AABB& aabb, float mass,
            const Vector3f& localPosition = Vector3f(0.f),
            const Quaternion& localRotation = Quaternion(1.f, 0.f, 0.f, 0.f)) override;

        /**
         * @brief Add a sphere collider to a body (by radius).
         * @param body Parent body.
         * @param radius Sphere radius (m).
         * @param mass Mass contribution (kg).
         * @param localPosition Local offset.
         * @param localRotation Local rotation.
         * @return New collider.
         */
        Collider* AddSphereCollider(RigidBody* body, float radius, float mass,
            const Vector3f& localPosition = Vector3f(0.f),
            const Quaternion& localRotation = Quaternion(1.f, 0.f, 0.f, 0.f)) override;

        /**
         * @brief Add a sphere collider fit from an AABB.
         * @param body Parent body.
         * @param aabb Local-space AABB used to derive radius.
         * @param mass Mass contribution (kg).
         * @param localPosition Local offset.
         * @param localRotation Local rotation.
         * @return New collider.
         */
        Collider* AddSphereCollider(RigidBody* body, const AABB& aabb, float mass,
            const Vector3f& localPosition = Vector3f(0.f),
            const Quaternion& localRotation = Quaternion(1.f, 0.f, 0.f, 0.f)) override;

        /**
         * @brief Add a capsule collider to a body.
         * @param body Parent body.
         * @param radius Capsule radius (m).
         * @param height Cylinder height between hemispheres (m).
         * @param mass Mass contribution (kg).
         * @param localPosition Local offset.
         * @return New collider.
         */
        Collider* AddCapsuleCollider(RigidBody* body, float radius, float height, float mass,
            const Vector3f& localPosition = Vector3f(0.f)) override;

        /**
         * @brief Register a collision callback.
         * @param cb Callback pointer (non-owning).
         */
        void RegisterCollisionCallback(CollisionCallback* cb) override;

        /**
         * @brief Unregister a collision callback.
         * @param cb Callback pointer.
         */
        void UnregisterCollisionCallback(CollisionCallback* cb) override;

    private:

        std::vector<std::unique_ptr<SimpleRigidBody>> m_bodies;     /// Owned rigid bodies.
        std::vector<std::unique_ptr<SimpleCollider>> m_colliders;   /// Owned colliders.

        std::vector<ContactManifold> m_contactManifolds;            /// Manifolds produced this step.
        std::vector<CollisionCallback*> m_collisionCallBacks;       /// Registered collision callbacks.

        bool m_hasStarted;                                          /// True after first update/step.
        Vector3f m_gravity;                                         /// Gravity vector (m/s^2).

        /**
         * @brief Print a labeled 3D vector (debug).
         * @param variable Label.
         * @param v Vector value.
         */
        void PrintVector(std::string variable, const Vector3f& v);

        /**
         * @brief Apply euler integration to a rigid body's physical properties.
         * @param body rigid body to integrate
         * @param dt delta time
         */
        void IntegrateBody(SimpleRigidBody* body, float dt);

        /**
         * @brief Rebuild broadphase AABBs for all colliders.
         *
         * Updates cached bounds used by broadphase overlap tests.
         */
        void RecomputeBroadphaseAABBs();

        /**
         * @brief Test broadphase overlap between two AABBs.
         * @param a First AABB.
         * @param b Second AABB.
         * @return True if overlapping.
         */
        bool BroadphaseCollisionDetection(const AABB& a, const AABB& b);

        /**
         * @brief Run narrowphase on a possible colliding pair.
         * @param firstCol First collider.
         * @param secondCol Second collider.
         */
        void NarrowphaseCollisionDetection(SimpleCollider* firstCol, SimpleCollider* secondCol);

        /**
         * @brief Decide whether each box is AABB or OBB before calling the appropriate narrowphase function.
         */
        void BoxToBoxCollision(BoxCollider* colA, BoxCollider* colB);

        /**
         * @brief Decide whether the box is AABB or OBB before calling the appropriate narrowphase function.
         */
        void BoxToSphereCollision(BoxCollider* colBox, SphereCollider* colSphere);

        // void BoxToCapsuleCollision(BoxCollider* boxCol, CapsuleCollider* capsuleCol);

        /**
         * @brief Sphere–Sphere narrowphase.
         */
        void SphereToSphereCollision(SphereCollider* colA, SphereCollider* colB);

        /**
         * @brief AABB–AABB overlap (box colliders in AABB mode).
         */
        void AABBToAABBCollision(BoxCollider* colA, BoxCollider* colB);

        /**
         * @brief AABB–Sphere overlap.
         */
        void AABBToSphereCollision(BoxCollider* colBox, SphereCollider* colSphere);

        /**
         * @brief OBB–Sphere narrowphase.
         */
        void OBBToSphereCollision(BoxCollider* boxCol, SphereCollider* sphereCol);

        /**
         * @brief OBB–OBB narrowphase.
         */
        void OBBToOBBCollision(BoxCollider* colA, BoxCollider* colB);

        /*
        void AABBToCapsuleCollision(BoxCollider* boxCol, CapsuleCollider* capsuleCol);
        void SphereToCapsuleCollision(SphereCollider* sphereCol, CapsuleCollider* capsuleCol);
        void OBBToCapsuleCollision(BoxCollider* boxCol, CapsuleCollider* capCol);
        */

        /**
         * @brief Append a contact to a manifold.
         * @param M Manifold to fill.
         * @param p Contact position (world).
         * @param n Contact normal (world, from A to B).
         * @param pen Penetration depth (m).
         */
        void AddContact(ContactManifold& M, const Vector3f& p, const Vector3f& n, float pen);

        /**
         * @brief Compare two points with tolerance.
         * @param a First point.
         * @param b Second point.
         * @param tol Distance tolerance.
         * @return True if within tolerance.
         */
        bool AlmostSamePoint(const Vector3f& a, const Vector3f& b, float tol = 1e-4f);

        /**
         * @brief Clip a polygon against a plane.
         * @param pn Plane normal.
         * @param pd Plane distance (dot(pn,x)=pd).
         * @param inPts Input points.
         * @param inCount Input count.
         * @param outPts Output buffer.
         * @return Number of output points.
         */
        int ClipPolygonAgainstPlane(const Vector3f& pn, float pd, const Vector3f* inPts, int inCount, Vector3f* outPts);

        /**
         * @brief Resolve a single contact manifold.
         * @param m Manifold to resolve.
         */
        void ResolveCollision(const ContactManifold& m);

        /**
         * @brief Apply linear impulse to a rigid body.
         * @param body rigid body
         * @param impulse Linear impulse to apply
         */
        void ApplyLinearImpulseToBody(Physics::RigidBody* body, const Vector3f& impulse);

        /**
         * @brief Apply angular impulse to a rigid body.
         * @param body rigid body
         * @param impulse Angular impulse to apply
         * @param point on the body where the impulse is applied.
         */
        void ApplyAngularImpulseToBody(Physics::RigidBody* body, const Vector3f& impulse, const Vector3f& point);

        /**
         * @brief Closest point on segment AB to point P.
         * @param a Segment start.
         * @param b Segment end.
         * @param p Query point.
         * @return Closest point on segment.
         */
        Vector3f ClosestPointOnSegment(const Vector3f& a, const Vector3f& b, const Vector3f& p);

        /**
         * @brief SAT between two OBBs.
         * @param boxA First box.
         * @param boxB Second box.
         * @param outNormal Minimum translation direction (world).
         * @param outPen Penetration depth.
         * @return True if overlapping.
         */
        inline bool OBB_SAT(BoxCollider* boxA, BoxCollider* boxB, Vector3f& outNormal, float& outPen);

    };

} // namespace Physics::Backend

#endif
