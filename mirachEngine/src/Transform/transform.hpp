#ifndef TRANSFORM_HPP
#define TRANSFORM_HPP

#include "../Math/math.hpp"

/**
 * @class Transform
 * @brief Represents position, rotation, and scale of an object in 3D space.
 *
 * Encapsulates spatial transformations using position vectors, scale, and rotation (as a quaternion).
 * Provides a method to compute the model matrix used for rendering and physics.
 */
class Transform
{
public:
    /**
     * @brief Default constructor initializes position to (0,0,0), scale to (1,1,1), and rotation to identity.
     */
    Transform();

    /**
     * @brief Computes and returns the model (world) transformation matrix.
     * @return Model matrix as Matrix4f.
     */
    Matrix4f GetModelMatrix() const;

    /**
     * @brief Sets the world-space position of the object.
     * @param x X coordinate.
     * @param y Y coordinate.
     * @param z Z coordinate.
     */
    void SetPosition(float x, float y, float z);

    void SetPosition(const Vector3f& position);

    /**
     * @brief Sets the object's rotation in Euler degrees.
     * @param x Rotation around X (pitch).
     * @param y Rotation around Y (yaw).
     * @param z Rotation around Z (roll).
     */
    void SetRotation(float x, float y, float z);

    /**
     * @brief Sets the object's rotation using a quaternion.
     * @param q Quaternion representing orientation.
     */
    void SetRotation(const Quaternion& q);

    /**
     * @brief Sets the object's local scale.
     * @param x Scale along X axis.
     * @param y Scale along Y axis.
     * @param z Scale along Z axis.
     */
    void SetScale(float x, float y, float z);

    void SetScale(const Vector3f& scale);

    /**
     * @brief Sets whether the object's up axis should be Y (true) or Z (false).
     * @param up True for Y-up, false for Z-up.
     */
    void SetYAxisUp(bool up);

    /**
     * @brief Retrieves the current position.
     * @return Position as a Vector3f.
     */
    Vector3f GetPosition() const;

    /**
     * @brief Retrieves the current rotation in Euler angles (degrees).
     * @return Rotation as Vector3f (pitch, yaw, roll).
     */
    Vector3f GetRotation() const;

    Quaternion GetQuaternionRotation() const;

    /**
     * @brief Retrieves the current scale vector.
     * @return Scale as a Vector3f.
     */
    Vector3f GetScale() const;

    /**
     * @brief Returns whether the object's up axis is Y.
     * @return True if Y is up, false if Z is up.
     */
    bool IsYAxisUp() const;

private:
    /**
     * @brief Wraps a rotation angle to the range [0, 360).
     * @param axis Angle in degrees.
     * @return Normalized angle.
     */
    float ResetRotation(float axis);

    /**
     * @brief Recalculates the cached model matrix based on current position, rotation, and scale.
     */
    void UpdateModelMatrix();

    Matrix4f m_modelMatrix;     ///< Cached 4x4 model matrix used for rendering and physics.
    Vector3f m_position;       ///< World-space position of the object.
    Quaternion m_rotation;     ///< Orientation represented as a unit quaternion.
    Vector3f m_scale;          ///< Local scale vector.

    bool m_yAxisUp;              ///< Flag indicating whether Y (true) or Z (false) is the up axis.
};

#endif // TRANSFORM_HPP
