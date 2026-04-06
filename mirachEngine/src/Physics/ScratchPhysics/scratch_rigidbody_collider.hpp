#include "../physics.hpp"
#include <vector>

/**
 * @enum ShapeType
 * @brief Supported collider shape kinds.
 */
enum class ShapeType
{
    SPHERE,      /// Sphere collider
    BOX,         /// Box/OBB or AABB collider
    CAPSULE,     /// Capsule collider (WIP)
    CYLINDER,    /// Cylinder collider (not implemented)
    MESH,        /// Triangle mesh collider (not implemented)
    HEIGHTFIELD  /// Heightfield/terrain collider (not implemented)
};

namespace Physics::Backend
{
    class SimpleCollider;

    /**
     * @class SimpleRigidBody
     * @brief Concrete rigid body used by the SimpleWorld backend.
     */
    class SimpleRigidBody : public RigidBody
    {
    public:
        /**
         * @brief Construct a rigid body.
         * @param position Initial world position.
         * @param rotation Initial world rotation.
         * @param type Body type.
         */
        SimpleRigidBody(const Vector3f& position, const Quaternion& rotation, const BodyType& type);

        /** 
         * @brief Set world position. 
         * @param pos Position in meters. 
         */
        void SetPosition(const Vector3f& pos) override;

        /** 
         * @brief Get world position. 
         * @return Position in meters. 
         */
        Vector3f GetPosition() const override;

        /** 
         * @brief Set world orientation. 
         * @param rot Unit quaternion. 
         */
        void SetOrientation(const Quaternion& rot) override;

        /** 
         * @brief Get world orientation. 
         * @return Unit quaternion. 
         */
        Quaternion GetOrientation() const override;

        /** 
         * @brief Apply continuous force at COM. 
         * @param force Force in N. 
         */
        void ApplyLinearForce(const Vector3f& force) override;

        /** 
         * @brief Apply instantaneous linear impulse. 
         * @param impulse Impulse in N·s. 
         */
        void ApplyLinearImpulse(const Vector3f& impulse) override;

        /**
         * @brief Apply angular impulse at point.
         * @param impulse Angular/torque impulse vector.
         * @param point World-space application point.
         */
        void ApplyAngularImpulse(const Vector3f& impulse, const Vector3f& point) override;

        /** 
         * @brief Set linear velocity. 
         * @param vel Velocity in m/s. 
         */
        void SetLinearVelocity(const Vector3f& vel) override;

        /** 
         * @brief Get linear velocity. 
         * @return Velocity in m/s. 
         */
        Vector3f GetLinearVelocity() const override;

        /** 
         * @brief Set angular velocity. 
         * @param w Angular velocity in rad/s. 
         */
        void SetAngularVelocity(const Vector3f& w) override;

        /** 
         * @brief Get angular velocity. 
         * @return Angular velocity in rad/s. 
         */
        Vector3f GetAngularVelocity() const override;

        /**
         * @brief Get net force.
         * @return Net force in N.
         */
        Vector3f GetNetForce() const;

        /**
         * @brief Set net force.
         * @param netForce Net force in N.
         */
        void SetNetForce(const Vector3f& netForce);

        /**
         * @brief Get linear acceleration. 
         * @return linear acceleration in m/s^2.
         */
        Vector3f GetLinearAcceleration() const;

        /**
         * @brief Set linear acceleration.
         * @param a linear acceleration in m/s^2.
         */
        void SetLinearAcceleration(const Vector3f& a);

        /**
         * @brief Project a vector onto the angular axes that are free to rotate.
         * @param v Input vector.
         * @return Vector masked to free axes.
         */
        Vector3f AngularVectorToFreeAxis(const Vector3f& v);

        /** 
         * @brief Enable/disable gravity. 
         * @param enabled True to enable. 
         */
        void EnableGravity(bool enabled) override;

        /** 
         * @brief Is gravity enabled? 
         * @return True if enabled. 
         */
        bool IsGravityEnabled() const override;

        /** 
         * @brief Is this body dynamic? 
         * @return True if BodyType::DYNAMIC. 
         */
        bool IsDynamic() const override;

        /** 
         * @brief Set total mass. 
         * @param m Mass in kg. 
         */
        void SetTotalMass(float m) override;

        /** 
         * @brief Get total mass. 
         * @return Mass in kg. 
         */
        float GetTotalMass() const override;

        /** 
         * @brief Add to total mass. 
         * @param m Delta mass in kg. 
         */
        void AddMass(float m);

        /** 
         * @brief Get body type. 
         * @return DYNAMIC/KINEMATIC. 
         */
        BodyType GetType() const override;

        /**
         * @brief Set free/locked angular axes.
         * @param axis 1=free, 0=locked per axis.
         */
        void SetAngularFreeAxis(const Vector3f& axis) override;

        /** 
         * @brief Set angular damping. 
         * @param d Damping factor. 
         */
        void SetAngularDamping(float d) override;

        /**
         * @brief Compute inverse inertia tensor (local/world).
         */
        void CalculateInverseInertiaTensor();

        /**
         * @brief Get COM in world space.
         * @return World-space COM.
         */
        Vector3f GetWorldCentreOfMass() const override;

        /**
         * @brief Get attached colliders.
         * @return List of collider pointers.
         */
        std::vector<Collider*> GetColliders() override;

        /**
         * @brief Attach a collider to this body.
         * @param col Collider pointer.
         */
        void AddCollider(SimpleCollider* col);

        /**
         * @brief Get cached broadphase AABB.
         * @return Broadphase AABB.
         */
        AABB GetBroadphaseAABB() const;

        /**
         * @brief Expand broadphase AABB to include another.
         * @param other Other AABB.
         */
        void BroadphaseExpand(const AABB& other);

        /**
         * @brief Reset broadphase AABB to empty.
         */
        void BroadphaseReset();

        /**
         * @brief Get inverse inertia tensor in world space.
         * @return 3x3 inverse inertia tensor (world).
         */
        Matrix3f GetInverseInertiaTensorWorld() const;

        /**
         * @brief Set inverse inertia tensor in world space.
         * @param m 3x3 inverse inertia tensor (world).
         */
        void SetInverseInertiaTensorWorld(const Matrix3f& m);

        /**
         * @brief Get inverse inertia tensor in local space.
         * @return 3x3 inverse inertia tensor (local).
         */
        Matrix3f GetInverseInertiaTensorLocal() const;

    private:
        /**
         * @brief Shift body so the given COM becomes the origin.
         * @param com COM in local/body space.
         */
        void RecentreBodyToCOM(const Vector3f& com);

        /**
         * @brief Compute local COM from attached colliders/masses.
         */
        void CalculateLocalCentreOfMass();

        Vector3f m_position;              /// World position
        Quaternion m_rotation;            /// World orientation
        BodyType m_type;                  /// Body type

        Vector3f m_velocity;              /// Linear velocity
        Vector3f m_acceleration;          /// Linear acceleration (accumulator)
        Vector3f m_netForce;              /// Accumulated force

        Vector3f m_localCOM;              /// COM in local/body space

        Vector3f m_angularVelocity;       /// Angular velocity (rad/s)
        float m_angularDamping;           /// Angular damping

        Matrix3f m_inverseInertiaTensorLocal; /// Inverse inertia (local)
        Matrix3f m_inverseInertiaTensorWorld; /// Inverse inertia (world)

        Vector3f m_angularFreeAxis;       /// 1=free, 0=locked per axis

        AABB m_broadphaseAABB;            /// Broadphase bounds
        float m_mass;                     /// Total mass (kg)
        bool m_gravityEnabled;            /// Gravity flag

        std::vector<SimpleCollider*> m_colliders; /// Attached colliders
    };

    /**
     * @class SimpleCollider
     * @brief Base collider with shared material/pose logic.
     */
    class SimpleCollider : public Collider
    {
    public:
        /**
         * @brief Construct base collider.
         * @param body Parent rigid body.
         * @param localPos Local position.
         * @param localRot Local rotation.
         */
        SimpleCollider(RigidBody* body, const Vector3f& localPos, const Quaternion& localRot);

        /**
         * @brief Set collision mask.
         * @param m Mask bits.
         */
        void SetCollisionMask(uint16_t m) override;

        /**
         * @brief Set friction coefficient.
         * @param f Coefficient.
         */
        void SetFriction(float f) override;

        /**
         * @brief Get friction coefficient.
         * @return Coefficient.
         */
        float GetFriction() const override;

        /**
         * @brief Set restitution coefficient.
         * @param b Bounciness [0..1].
         */
        void SetBounciness(float b) override;

        /**
         * @brief Get restitution coefficient.
         * @return Bounciness [0..1].
         */
        float GetBounciness() const override;

        /**
         * @brief Set category bits.
         * @param c Category.
         */
        void SetCategoryBits(uint16_t c) override;

        /**
         * @brief Get category bits.
         * @return Category.
         */
        uint16_t GetCategoryBits() const override;

        /**
         * @brief Set mass contribution.
         * @param m Mass in kg.
         */
        void SetMass(float m) override;

        /**
         * @brief Get mass contribution.
         * @return Mass in kg.
         */
        float GetMass() const override;

        /**
         * @brief Add to mass contribution.
         * @param m Delta mass in kg.
         */
        void AddMass(float m);

        /**
         * @brief World-space center of this collider.
         * @return Position.
         */
        Vector3f GetWorldCentre() override;

        /**
         * @brief World-space rotation of this collider.
         * @return 3x3 rotation.
         */
        Matrix3f GetCompositeRotation() override;

        /**
         * @brief Parent rigid body.
         * @return Body pointer.
         */
        RigidBody* GetParentBody() override;

        /**
         * @brief Virtual destructor.
         */
        virtual ~SimpleCollider() = default;

        /**
         * @brief Calculate local-space inertia tensor for this shape.
         * @return 3x3 inertia tensor.
         */
        virtual Matrix3f CalculateInertiaTensor() = 0;

        /**
         * @brief Ray intersection test.
         * @param ray Input ray (world).
         * @param outT Hit distance (if true).
         * @return True if hit.
         */
        virtual bool RayIntersect(const Ray& ray, float& outT) = 0;

        /**
         * @brief Recompute broadphase AABB.
         * @param margin Extra AABB padding.
         */
        virtual void RecomputeBroadPhaseAABB(float margin) = 0;

        /**
         * @brief Get this collider's shape type.
         * @return ShapeType value.
         */
        ShapeType GetShapeType() const;

        /**
         * @brief Get local position relative to body.
         * @return Local position.
         */
        Vector3f GetLocalPosition() const;

        /**
         * @brief Adjust local position by COM shift.
         * @param com New body COM (local).
         */
        void AdjustLocalPositionToCOM(const Vector3f& com);

        /**
         * @brief Get local rotation relative to body.
         * @return Local rotation.
         */
        Quaternion GetLocalRotation() const;

        /**
         * @brief Get cached broadphase AABB.
         * @return AABB.
         */
        AABB GetBroadphaseAABB() const;

    protected:
        /**
         * @brief Fill broadphase AABB using oriented extents.
         * @param halfExtents Local half extents.
         */
        void CalculateBroadphaseAABBThroughExtents(Vector3f halfExtents);

        SimpleRigidBody* m_parentBody = nullptr; /// Parent body (cached)

        ShapeType m_shapeType;       /// Shape kind
        AABB m_broadphaseAABB;       /// Broadphase bounds

        float m_mass;                /// Mass contribution (kg)
        float m_friction;            /// Friction coefficient
        float m_bounciness;          /// Restitution (0..1)

        Vector3f m_localPosition;    /// Local offset
        Quaternion m_localRotation;  /// Local rotation

    private:
        uint16_t m_category;         /// Category bits
        uint16_t m_mask;             /// Mask bits
    };

    /**
     * @class SphereCollider
     * @brief Sphere shape collider.
     */
    class SphereCollider : public SimpleCollider
    {
    public:
        /**
         * @brief Construct a sphere collider.
         * @param body Parent body.
         * @param mass Mass (kg).
         * @param radius Radius (m).
         * @param localPosition Local offset.
         * @param localRot Local rotation.
         */
        SphereCollider(RigidBody* body, float mass, float radius, const Vector3f& localPosition, const Quaternion& localRot);

        /**
         * @brief Compute inertia tensor.
         * @return 3x3 tensor.
         */
        Matrix3f CalculateInertiaTensor() override;

        /**
         * @brief Recompute broadphase AABB.
         * @param margin Padding.
         */
        void RecomputeBroadPhaseAABB(float margin) override;

        /**
         * @brief Ray-sphere intersection.
         * @param ray World ray.
         * @param outT Hit distance if true.
         * @return True if hit.
         */
        bool RayIntersect(const Ray& ray, float& outT) override;

        /**
         * @brief Sphere radius.
         * @return Radius in meters.
         */
        float GetRadius() const;

    private:
        float m_radius; /// Radius (m)
    };

    /**
     * @struct FaceRef
     * @brief Reference face data for OBB contact generation.
     */
    struct FaceRef
    {
        Vector3f n;        /// World normal (A->B)
        Vector3f center;   /// Face center (world)
        Vector3f u, v;     /// In-plane unit axes
        float halfU;       /// Half-width along u
        float halfV;       /// Half-width along v
    };

    /**
     * @class BoxCollider
     * @brief Box collider (AABB/OBB).
     */
    class BoxCollider : public SimpleCollider
    {
    public:
        /**
         * @brief Construct a box collider.
         * @param body Parent body.
         * @param mass Mass (kg).
         * @param halfExtents Half extents (local).
         * @param localPos Local offset.
         * @param localRot Local rotation.
         */
        BoxCollider(RigidBody* body, float mass, const Vector3f& halfExtents, const Vector3f& localPos, const Quaternion& localRot);

        /**
         * @brief Compute inertia tensor.
         * @return 3x3 tensor.
         */
        Matrix3f CalculateInertiaTensor() override;

        /**
         * @brief Recompute broadphase AABB.
         * @param margin Padding.
         */
        void RecomputeBroadPhaseAABB(float margin) override;

        /**
         * @brief Ray-OBB intersection.
         * @param ray World ray.
         * @param outT Hit distance if true.
         * @return True if hit.
         */
        bool RayIntersect(const Ray& ray, float& outT) override;

        /**
         * @brief Detect static axis alignment and flag OBB/AABB mode.
         */
        void CheckStaticBoxAlignment();

        /**
         * @brief Support point along direction for OBB.
         * @param dir World direction.
         * @return Extreme point in +dir.
         */
        Vector3f SupportPointOBB(const Vector3f& dir);

        /**
         * @brief Get a face of the OBB.
         * @param axis 0/1/2 (x/y/z).
         * @param sign +1 front, -1 back.
         * @param center Face center (out).
         * @param u First face axis (out).
         * @param v Second face axis (out).
         * @param halfU Half-size along u (out).
         * @param halfV Half-size along v (out).
         * @param verts Four face verts (out).
         */
        void GetOBBFace(int axis, int sign,
            Vector3f& center, Vector3f& u, Vector3f& v,
            float& halfU, float& halfV,
            Vector3f verts[4]);

        /**
         * @brief Build reference face data for a normal.
         * @param n World normal.
         * @return Reference face information.
         */
        FaceRef BuildReferenceFace(const Vector3f& n);

        /**
         * @brief Is the box oriented (OBB) vs axis-aligned?
         * @return True if OBB.
         */
        bool IsOriented() const;

        /**
         * @brief Get half extents.
         * @return Local half extents.
         */
        Vector3f GetHalfExtents() const;

    private:
        Vector3f m_halfExtents; /// Half extents (local)
        bool m_oriented;        /// True = OBB, false = AABB
    };

    /**
     * @class CapsuleCollider
     * @brief Capsule collider (cylinder with hemispherical ends).
     */
    class CapsuleCollider : public SimpleCollider
    {
    public:
        /**
         * @brief Construct a capsule collider.
         * @param body Parent body.
         * @param localPosition Local offset.
         * @param mass Mass (kg).
         * @param radius Radius (m).
         * @param cylinderLength Cylinder length (between hemisphere centers).
         */
        CapsuleCollider(RigidBody* body, const Vector3f& localPosition, float mass, float radius, float cylinderLength);

        /**
         * @brief Compute inertia tensor.
         * @return 3x3 tensor.
         */
        Matrix3f CalculateInertiaTensor() override;

        /**
         * @brief Recompute broadphase AABB.
         * @param margin Padding.
         */
        void RecomputeBroadPhaseAABB(float margin) override;

        /**
         * @brief Ray-capsule intersection.
         * @param ray World ray.
         * @param outT Hit distance if true.
         * @return True if hit.
         */
        bool RayIntersect(const Ray& ray, float& outT) override;

    private:
        float m_radius;          /// Radius (m)
        float m_cylinderLength;  /// Cylinder length (m)
    };
}
