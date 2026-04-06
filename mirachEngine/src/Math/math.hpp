#ifndef MATH_HPP
#define MATH_HPP
/**
 * @file math.hpp
 * @brief Provides math utilities, vector/matrix types, transformation functions, and AABB logic using GLM.
 */

 // === Math Constants ===

 /// Pi constant in float precision
constexpr float PI = 3.14159265358979323846f;

/// 2*Pi constant in float precision
constexpr float TWO_PI = PI * 2.0f;

// === GLM Includes ===
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include <glm/mat3x3.hpp>
#include <glm/mat4x4.hpp>
#include <glm/gtc/quaternion.hpp>


// === Common Type Aliases ===

using Vector2f = glm::vec2; // 2D float vector
using Vector3f = glm::vec3; // 3D float vector
using Vector4f = glm::vec4; // 4D float vector

using Matrix3f = glm::mat3; // 3x3 float matrix
using Matrix4f = glm::mat4; // 4x4 float matrix

using Quaternion = glm::quat; // Quaternion type

// === Float functions ===

float Absf(float x);

// === Projection and View ===

/// Creates a perspective projection matrix
Matrix4f Perspective(float fovRadians, float aspectRatio, float nearPlane, float farPlane);

/// Creates a view matrix looking from 'eye' to 'center' with 'up' direction
Matrix4f LookAt(const Vector3f& eye, const Vector3f& center, const Vector3f& up);

// === Scalar and Vector Utilities ===

/// Clamps a float between minVal and maxVal
float Clamp(float value, float minVal, float maxVal);

Matrix3f Transpose(const Matrix3f& M);

Matrix3f Diagonal3x3(const Vector3f& v);

Vector3f DiagonalFromMat3(const Matrix3f& M);

/// Converts a quaternion to a 3x3 rotation matrix
Matrix3f ToMat3(const Quaternion& q);

/// Normalizes a 3D vector


Matrix3f Outer(const Vector3f& a, const Vector3f& b);

/// Cross product of two 3D vectors
Vector3f Cross(const Vector3f& a, const Vector3f& b);

/// Dot product of two 3D vectors
float Dot(const Vector3f& a, const Vector3f& b);

/// Dot product of two 2D vectors
float Dot(const Vector2f& a, const Vector2f& b);

/// Converts degrees to radians
float ToRadians(float degrees);

/// Converts radians to degrees
float ToDegrees(float radians);

/// Returns the length of a 2D vector
float Length(const Vector2f& v);

/// Returns the length of a 3D vector
float Length(const Vector3f& v);

float LengthSq(const Vector3f& v);

float Squared(float x);

/// Returns the length of a 4D vector
float Length(const Vector4f& v);

/// Returns the Euclidean distance between two 3D vectors
float Distance(const Vector3f& a, const Vector3f& b);

/// Returns the identity matrix
Matrix4f Identity();

/// Returns the 3x3 identity matrix
Matrix3f Identity3x3();

bool IsIdentity(const Matrix3f& M, float eps = 1e-4f);

// === Transformations ===

/// Applies translation to a matrix
Matrix4f Translate(const Matrix4f& matrix, const Vector3f& v);

/// Applies scale to a matrix
Matrix4f Scale(const Matrix4f& matrix, const Vector3f& v);

/// Applies rotation to a matrix
Matrix4f Rotate(const Matrix4f& matrix, float angleRad, const Vector3f& axis);

/// Transforms a 3D vector using a matrix
Vector3f TransformVector(const Matrix4f& m, const Vector3f& v);

// === Quaternion Utilities ===

/// Converts a quaternion to Euler angles (in degrees)
Vector3f QuaternionToEulerDegrees(const Quaternion& q);

/// Converts Euler angles (in degrees) to a quaternion
Quaternion EulerDegreesToQuaternion(const Vector3f& degrees);

Quaternion ToQuaternion(const Matrix3f& M);

/// Creates a quaternion from scalar-first values (w, x, y, z)
Quaternion MakeQuaternion(float w, float x, float y, float z);

/// Converts a quaternion to a 4x4 matrix
Matrix4f ToMat4(const Quaternion& q);

/// Returns the inverse of a quaternion
Quaternion Inverse(const Quaternion& q);

Matrix3f Inverse(const Matrix3f& v);

/// Returns the normalized version of a quaternion
Quaternion Normalize(const Quaternion& q);

Vector3f Normalize(const Vector3f& v);

Quaternion AddSmallRotation(const Quaternion& q, const Vector3f& dTheta);

Quaternion AddRotation(const Quaternion& q, const Vector3f& dtheta);

Quaternion Conjugate(const Quaternion& q);

Quaternion AngleAxis(float angleRad, const Vector3f& axis);

Quaternion YawQuat(float yawRad);

float YawFromQuat(const Quaternion& q);

// Strip world-yaw from q: tilt = conj(yawOnly(q)) * q
Quaternion RemoveYaw(const Quaternion& q);

void QuaternionToAxisAngle(const Quaternion& qIn, Vector3f& axis, float& angle);


// === Interpolation ===

/// Performs linear interpolation between two scalar values
float Lerp(float a, float b, float t);

/// Performs linear interpolation between two vectors
Vector3f Lerp(const Vector3f& a, const Vector3f& b, float t);

/// Interpolates between angles and wraps around 360 degrees
float LerpAngle(float a, float b, float t);

// === AABB ===

/**
 * @struct AABB
 * @brief Represents an axis-aligned bounding box.
 */
struct AABB
{
    Vector3f min; // Minimum corner of the bounding box
    Vector3f max; // Maximum corner of the bounding box

    /// Constructs an invalid AABB by resetting its values
    AABB();

    /// Resets the AABB to an empty state
    void Reset();

    bool IsEmpty() const;

    /// Expands the AABB to include the specified point
    void Expand(const Vector3f& point);

    void Expand(const AABB& other);

    /// Transforms this AABB by a transformation matrix and returns the result
    AABB Transform(const Matrix4f& modelMatrix) const;
};

#endif // MATH_HPP
