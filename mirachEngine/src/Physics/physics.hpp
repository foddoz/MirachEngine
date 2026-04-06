#ifndef PHYSICS_HPP
#define PHYSICS_HPP

#include "../Math/math.hpp"
#include <memory>
#include <vector>
#include <unordered_set>

/**
 * @file physics.hpp
 * @brief Core physics interfaces: bodies, colliders, world, and contacts.
 */

 /**
  * @enum BodyType
  * @brief Type of physics body.
  */
enum class BodyType
{
    DYNAMIC,   /// Fully simulated by physics
    KINEMATIC  /// User/animation driven, not by forces
};

namespace Physics
{

    class RigidBody;

    /**
     * @class Collider
     * @brief Base interface for collision shapes attached to a rigid body.
     */
    class Collider
    {
    public:
        /**
         * @brief Virtual destructor.
         */
        virtual ~Collider() {}

        /**
         * @brief Set the collision mask bitfield.
         * @param maskBits Mask of categories this collider can collide with.
         */
        virtual void SetCollisionMask(uint16_t maskBits) = 0;

        /**
         * @brief Set the friction coefficient.
         * @param coefficient Friction value (e.g., 0..1).
         */
        virtual void SetFriction(float coefficient) = 0;

        /**
         * @brief Get the friction coefficient.
         * @return Friction value.
         */
        virtual float GetFriction() const = 0;

        /**
         * @brief Set the bounciness / restitution.
         * @param bounciness Value in [0, 1].
         */
        virtual void SetBounciness(float bounciness) = 0;

        /**
         * @brief Get the bounciness / restitution.
         * @return Value in [0, 1].
         */
        virtual float GetBounciness() const = 0;

        /**
         * @brief Set this collider's category bits.
         * @param categoryBits Category bitfield.
         */
        virtual void SetCategoryBits(uint16_t categoryBits) = 0;

        /**
         * @brief Get this collider's category bits.
         * @return Category bitfield.
         */
        virtual uint16_t GetCategoryBits() const = 0;

        /**
         * @brief Set this collider's mass contribution.
         * @param m Mass in kilograms.
         */
        virtual void SetMass(float m) = 0;

        /**
         * @brief Get this collider's mass contribution.
         * @return Mass in kilograms.
         */
        virtual float GetMass() const = 0;

        /**
         * @brief Get world-space center of this collider.
         * @return World-space position.
         */
        virtual Vector3f GetWorldCentre() = 0;

        /**
         * @brief Get world-space rotation of this collider.
         * @return 3x3 rotation matrix.
         */
        virtual Matrix3f GetCompositeRotation() = 0;

        /**
         * @brief Get the parent rigid body.
         * @return Pointer to parent body (non-owning).
         */
        virtual RigidBody* GetParentBody() = 0;
    };

    /**
     * @class RigidBody
     * @brief Interface for simulated or kinematic bodies.
     */
    class RigidBody
    {
    public:
        /**
         * @brief Virtual destructor.
         */
        virtual ~RigidBody() {}

        /**
         * @brief Set world-space position.
         * @param pos Position in meters.
         */
        virtual void SetPosition(const Vector3f& pos) = 0;

        /**
         * @brief Get world-space position.
         * @return Position in meters.
         */
        virtual Vector3f GetPosition() const = 0;

        /**
         * @brief Set world-space orientation.
         * @param rot Unit quaternion.
         */
        virtual void SetOrientation(const Quaternion& rot) = 0;

        /**
         * @brief Get world-space orientation.
         * @return Unit quaternion.
         */
        virtual Quaternion GetOrientation() const = 0;

        /**
         * @brief Apply continuous force at center of mass.
         * @param force Force in Newtons.
         */
        virtual void ApplyLinearForce(const Vector3f& force) = 0;

        /**
         * @brief Apply instantaneous linear impulse.
         * @param impulse Impulse in N·s.
         */
        virtual void ApplyLinearImpulse(const Vector3f& impulse) = 0;

        /**
         * @brief Apply instantaneous angular impulse at a point.
         * @param impulse Angular/torque impulse vector.
         * @param point World-space application point.
         */
        virtual void ApplyAngularImpulse(const Vector3f& impulse, const Vector3f& point) = 0;

        /**
         * @brief Set linear velocity.
         * @param vel Velocity in m/s.
         */
        virtual void SetLinearVelocity(const Vector3f& vel) = 0;

        /**
         * @brief Get linear velocity.
         * @return Velocity in m/s.
         */
        virtual Vector3f GetLinearVelocity() const = 0;

        /**
         * @brief Set angular velocity.
         * @param vel Angular velocity in rad/s.
         */
        virtual void SetAngularVelocity(const Vector3f& vel) = 0;

        /**
         * @brief Get angular velocity.
         * @return Angular velocity in rad/s.
         */
        virtual Vector3f GetAngularVelocity() const = 0;

        /**
         * @brief Enable or disable gravity.
         * @param enabled True to enable gravity.
         */
        virtual void EnableGravity(bool enabled) = 0;

        /**
         * @brief Check if gravity is enabled.
         * @return True if gravity is enabled.
         */
        virtual bool IsGravityEnabled() const = 0;

        /**
         * @brief Check if body is dynamic.
         * @return True if BodyType == DYNAMIC.
         */
        virtual bool IsDynamic() const = 0;

        /**
         * @brief Set total mass of the body.
         * @param mass Mass in kilograms.
         */
        virtual void SetTotalMass(float mass) = 0;

        /**
         * @brief Get total mass of the body.
         * @return Mass in kilograms.
         */
        virtual float GetTotalMass() const = 0;

        /**
         * @brief Get the body type.
         * @return STATIC, DYNAMIC, or KINEMATIC.
         */
        virtual BodyType GetType() const = 0;

        /**
         * @brief Set angular freedom per axis.
         * @param lock (1 = free, 0 = locked) per axis, backend-defined.
         */
        virtual void SetAngularFreeAxis(const Vector3f& lock) = 0;

        /**
         * @brief Set angular damping factor.
         * @param damping Non-negative damping value.
         */
        virtual void SetAngularDamping(float damping) = 0;

        /**
         * @brief Get all attached colliders.
         * @return List of collider pointers.
         */
        virtual std::vector<Collider*> GetColliders() = 0;

        /**
         * @brief Get world-space center of mass.
         * @return COM position.
         */
        virtual Vector3f GetWorldCentreOfMass() const = 0;
    };

    /**
     * @struct Ray
     * @brief Ray for raycasting.
     */
    struct Ray
    {
        Vector3f origin;    /// Ray start position
        Vector3f direction; /// Normalized direction
    };

    /**
     * @struct RaycastHit
     * @brief Raycast result.
     */
    struct RaycastHit
    {
        bool hit = false;                                    ///< True if any hit
        float distance = std::numeric_limits<float>::max();  ///< Distance to hit
        Vector3f point = Vector3f(0.f);                      ///< Hit point
        Collider* collider = nullptr;                        ///< Hit collider
        RigidBody* body = nullptr;                           ///< Hit body
    };

    /**
     * @struct ContactPoint
     * @brief Single contact point between two colliders.
     */
    struct ContactPoint
    {
        Vector3f position;   /// Contact position
        Vector3f normal;     /// Contact normal
        float penetration;   /// Penetration depth
    };

    /**
     * @struct ContactManifold
     * @brief Group of contact points between two colliders.
     */
    struct ContactManifold
    {
        Collider* colA = nullptr;  /// First collider
        Collider* colB = nullptr;  /// Second collider
        int count = 0;             /// Number of points
        ContactPoint points[4];    /// Up to 4 contacts
        bool hit = false;          /// True if colliding
    };

    /**
     * @class CollisionCallback
     * @brief Receives contact enter/persist/exit events for a body.
     */
    class CollisionCallback
    {
    public:
        /**
         * @brief Construct callback for a body.
         * @param rb Body to observe (non-owning).
         */
        CollisionCallback(RigidBody* rb);

        /**
         * @brief Virtual destructor.
         */
        virtual ~CollisionCallback() = default;

        /**
         * @brief Process all manifolds for this frame.
         * @param contactManifolds All contact manifolds from the world.
         */
        void Update(std::vector<ContactManifold> contactManifolds);

    protected:
        /**
         * @brief Called when contact with another body begins.
         * @param other The other body.
         * @param cm Current contact manifold.
         */
        virtual void OnContactEnter(RigidBody* other, const ContactManifold& cm) = 0;

        /**
         * @brief Called while contact with another body continues.
         * @param other The other body.
         * @param cm Current contact manifold.
         */
        virtual void OnContactPersist(RigidBody* other, const ContactManifold& cm) = 0;

        /**
         * @brief Called when contact with another body ends.
         * @param other The other body.
         */
        virtual void OnContactExit(RigidBody* other) = 0;

        std::unordered_set<RigidBody*> m_collidedBodies; /// Current contacting bodies
        RigidBody* m_body;                               /// Observed body (non-owning)
    };

    /**
     * @class World
     * @brief Manages bodies/colliders and advances simulation.
     */
    class World
    {
    public:
        /**
         * @brief Virtual destructor.
         */
        virtual ~World() {}

        /**
         * @brief Set world gravity.
         * @param gravity Acceleration (m/s^2).
         */
        virtual void SetGravity(const Vector3f& gravity) = 0;

        /**
         * @brief Create a rigid body.
         * @param position Initial world position.
         * @param rotation Initial world rotation.
         * @param type Body type (default DYNAMIC).
         * @return New body owned by the world.
         */
        virtual RigidBody* CreateRigidBody(const Vector3f& position,
            const Quaternion& rotation,
            BodyType type = BodyType::DYNAMIC) = 0;

        /**
         * @brief Remove and destroy a rigid body.
         * @param ptr Body to remove.
         */
        virtual void RemoveRigidBody(RigidBody* ptr) = 0;

        /**
         * @brief Step the simulation.
         * @param dt Delta time in seconds.
         */
        virtual void Update(float dt) = 0;

        /**
         * @brief Cast a ray into the world.
         * @param ray Ray origin/direction (direction normalized).
         * @param maxDistance Maximum distance.
         * @return Closest hit, or hit=false if none.
         */
        virtual RaycastHit Raycast(const Ray& ray,
            float maxDistance = std::numeric_limits<float>::max()) = 0;

        /**
         * @brief Add a box collider to a body.
         * @param body Parent body.
         * @param aabb AABB describing box size (local).
         * @param mass Mass contribution.
         * @param localPosition Local offset.
         * @param localRotation Local rotation.
         * @return New collider.
         */
        virtual Collider* AddBoxCollider(RigidBody* body, const AABB& aabb, float mass,
            const Vector3f& localPosition = Vector3f(0.f),
            const Quaternion& localRotation = Quaternion(1.f, 0.f, 0.f, 0.f)) = 0;

        /**
         * @brief Add a sphere collider (by radius).
         * @param body Parent body.
         * @param radius Sphere radius.
         * @param mass Mass contribution.
         * @param localPosition Local offset.
         * @param localRotation Local rotation.
         * @return New collider.
         */
        virtual Collider* AddSphereCollider(RigidBody* body, float radius, float mass,
            const Vector3f& localPosition = Vector3f(0.f),
            const Quaternion& localRotation = Quaternion(1.f, 0.f, 0.f, 0.f)) = 0;

        /**
         * @brief Add a sphere collider (fit from AABB).
         * @param body Parent body.
         * @param aabb AABB used to derive sphere size.
         * @param mass Mass contribution.
         * @param localPosition Local offset.
         * @param localRotation Local rotation.
         * @return New collider.
         */
        virtual Collider* AddSphereCollider(RigidBody* body, const AABB& aabb, float mass,
            const Vector3f& localPosition = Vector3f(0.f),
            const Quaternion& localRotation = Quaternion(1.f, 0.f, 0.f, 0.f)) = 0;

        /**
         * @brief Add a capsule collider.
         * @param body Parent body.
         * @param radius Capsule radius.
         * @param height Cylinder height (between hemispheres).
         * @param mass Mass contribution.
         * @param localPosition Local offset.
         * @return New collider.
         */
        virtual Collider* AddCapsuleCollider(RigidBody* body, float radius, float height, float mass,
            const Vector3f& localPosition = Vector3f(0.f)) = 0;

        /**
         * @brief Register a collision callback.
         * @param cb Callback pointer (non-owning).
         */
        virtual void RegisterCollisionCallback(CollisionCallback* cb) = 0;

        /**
         * @brief Unregister a collision callback.
         * @param cb Callback pointer.
         */
        virtual void UnregisterCollisionCallback(CollisionCallback* cb) = 0;
    };

    /**
     * @brief Create a new physics world instance.
     * @return Unique pointer to a world.
     */
    std::unique_ptr<World> CreatePhysicsWorld();

} // namespace Physics

#endif // PHYSICS_HPP
