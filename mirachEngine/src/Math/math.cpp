#include "math.hpp"

#include <cmath>

#include <glm/common.hpp>
#include <glm/geometric.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/norm.hpp>
#include <glm/gtx/quaternion.hpp>

float Absf(float x)
{
    return x < 0 ? -x : x;
}

// === Projection and View ===

/// Creates a perspective projection matrix
Matrix4f Perspective(float fovRadians, float aspectRatio, float nearPlane, float farPlane)
{
    return glm::perspective(fovRadians, aspectRatio, nearPlane, farPlane);
}

/// Creates a view matrix looking from 'eye' to 'center' with 'up' direction
Matrix4f LookAt(const Vector3f& eye, const Vector3f& center, const Vector3f& up)
{
    return glm::lookAt(eye, center, up);
}

// === Scalar and Vector Utilities ===

/// Clamps a float between minVal and maxVal
float Clamp(float value, float minVal, float maxVal)
{
    return glm::clamp(value, minVal, maxVal);
}

Matrix3f Transpose(const Matrix3f& M)
{
    return glm::transpose(M);
}

Matrix3f Diagonal3x3(const Vector3f& v)
{
    return Matrix3f(
        Vector3f(v.x, 0.f, 0.f),
        Vector3f(0.f, v.y, 0.f),
        Vector3f(0.f, 0.f, v.z)
    );
}

Vector3f DiagonalFromMat3(const Matrix3f& M)
{
    return Vector3f(M[0][0], M[1][1], M[2][2]);
}

/// Converts a quaternion to a 3x3 rotation matrix
Matrix3f ToMat3(const Quaternion& q)
{
    return glm::toMat3(q);
}

/// Normalizes a 3D vector
Vector3f Normalize(const Vector3f& v) 
{ 
    return glm::normalize(v); 
}

Matrix3f Outer(const Vector3f& a, const Vector3f& b)
{
    return glm::outerProduct(a, b);
}

/// Cross product of two 3D vectors
Vector3f Cross(const Vector3f& a, const Vector3f& b) 
{ 
    return glm::cross(a, b); 
}

/// Dot product of two 3D vectors
float Dot(const Vector3f& a, const Vector3f& b) 
{ 
    return glm::dot(a, b); 
}

/// Dot product of two 2D vectors
float Dot(const Vector2f& a, const Vector2f& b) 
{ 
    return glm::dot(a, b); 
}

/// Converts degrees to radians
float ToRadians(float degrees) 
{ 
    return glm::radians(degrees); 
}

/// Converts radians to degrees
float ToDegrees(float radians) 
{ 
    return glm::degrees(radians); 
}

/// Returns the length of a 2D vector
float Length(const Vector2f& v) 
{ 
    return glm::length(v); 
}

/// Returns the length of a 3D vector
float Length(const Vector3f& v) 
{ 
    return glm::length(v); 
}

float LengthSq(const Vector3f& v) 
{ 
    return glm::length2(v); 
}

float Squared(float x)
{
    return x * x;
}

/// Returns the length of a 4D vector
float Length(const Vector4f& v) 
{ 
    return glm::length(v); 
}

/// Returns the Euclidean distance between two 3D vectors
float Distance(const Vector3f& a, const Vector3f& b) 
{ 
    return glm::distance(a, b); 
}

/// Returns the identity matrix
Matrix4f Identity() 
{ 
    return Matrix4f(1.0f); 
}

/// Returns the 3x3 identity matrix
Matrix3f Identity3x3() 
{ 
    return Matrix3f(1.0f); 
}

bool IsIdentity(const Matrix3f& M, float eps)
{
    for (int i = 0; i < 3; i++)
        for (int j = 0; j < 3; j++)
            if (std::abs(M[i][j] - (i == j ? 1.f : 0.f)) > eps)
                return false;
    return true;
}

// === Transformations ===

/// Applies translation to a matrix
Matrix4f Translate(const Matrix4f& matrix, const Vector3f& v)
{
    return glm::translate(matrix, v);
}

/// Applies scale to a matrix
Matrix4f Scale(const Matrix4f& matrix, const Vector3f& v)
{
    return glm::scale(matrix, v);
}

/// Applies rotation to a matrix
Matrix4f Rotate(const Matrix4f& matrix, float angleRad, const Vector3f& axis)
{
    return glm::rotate(matrix, angleRad, axis);
}

/// Transforms a 3D vector using a matrix
Vector3f TransformVector(const Matrix4f& m, const Vector3f& v)
{
    return Vector3f(m * Vector4f(v, 1.0f));
}

// === Quaternion Utilities ===

/// Converts a quaternion to Euler angles (in degrees)
Vector3f QuaternionToEulerDegrees(const Quaternion& q)
{
    return glm::degrees(glm::eulerAngles(glm::normalize(q)));
}

/// Converts Euler angles (in degrees) to a quaternion
Quaternion EulerDegreesToQuaternion(const Vector3f& degrees)
{
    return glm::quat(glm::radians(degrees));
}

Quaternion ToQuaternion(const Matrix3f& M)
{
    return glm::quat(M);
}

/// Creates a quaternion from scalar-first values (w, x, y, z)
Quaternion MakeQuaternion(float w, float x, float y, float z)
{
    return glm::normalize(Quaternion(w, x, y, z));
}

/// Converts a quaternion to a 4x4 matrix
Matrix4f ToMat4(const Quaternion& q)
{
    return glm::toMat4(q);
}

/// Returns the inverse of a quaternion
Quaternion Inverse(const Quaternion& q) 
{
    return glm::inverse(q);
}

Matrix3f Inverse(const Matrix3f& v) 
{
    return glm::inverse(v);
}

/// Returns the normalized version of a quaternion
Quaternion Normalize(const Quaternion& q) 
{ 
    return glm::normalize(q); 
}

Quaternion AddSmallRotation(const Quaternion& q, const Vector3f& dTheta)
{
    Quaternion dq(1.f, 0.5f * dTheta.x, 0.5f * dTheta.y, 0.5f * dTheta.z);
    Quaternion out = Normalize(dq * q);
    return out;
}

Quaternion AddRotation(const Quaternion& q, const Vector3f& dtheta)
{
    float theta = Length(dtheta);
    Quaternion dq;
    if (theta < 1e-8f)
    {
        // tiny-angle fallback
        Vector3f half = 0.5f * dtheta;
        dq = Quaternion(1.0f, half.x, half.y, half.z);
    }
    else
    {
        float half = 0.5f * theta;
        float s = std::sin(half) / theta;
        dq = Quaternion(std::cos(half), s * dtheta.x, s * dtheta.y, s * dtheta.z);
    }
    return Normalize(dq * q); // left-multiply = world-space delta
}

Quaternion Conjugate(const Quaternion& q)
{
    return glm::conjugate(q);
}

Quaternion AngleAxis(float angleRad, const Vector3f& axis)
{
    // Make sure the axis is normalized
    Vector3f n = Normalize(axis);
    float halfAngle = angleRad * 0.5f;
    float s = std::sin(halfAngle);
    float c = std::cos(halfAngle);

    // Quaternion(w, x, y, z)
    return Quaternion(c, n.x * s, n.y * s, n.z * s);
}

Quaternion YawQuat(float yawRad) 
{
    return AngleAxis(yawRad, Vector3f(0, 1, 0));
}

float YawFromQuat(const Quaternion& q) 
{
    // Forward vector then project to XZ
    Vector3f f = glm::rotate(q, Vector3f(0, 0, 1));
    return std::atan2(f.x, f.z);
}

Quaternion RemoveYaw(const Quaternion& q) 
{
    float yaw = YawFromQuat(q);
    return Normalize(Conjugate(YawQuat(yaw)) * q);
}

void QuaternionToAxisAngle(const Quaternion& qIn, Vector3f& axis, float& angle)
{
    Quaternion q = glm::normalize(qIn);
    float w = glm::clamp(q.w, -1.0f, 1.0f);
    angle = 2.0f * std::acos(w);
    float s = std::sqrt(glm::max(1.0f - w * w, 0.0f));
    axis = (s > 1e-6f) ? Vector3f(q.x / s, q.y / s, q.z / s) : Vector3f(0, 1, 0);
}

// === Interpolation ===

/// Performs linear interpolation between two scalar values
float Lerp(float a, float b, float t)
{
    return glm::mix(a, b, t);
}

/// Performs linear interpolation between two vectors
Vector3f Lerp(const Vector3f& a, const Vector3f& b, float t)
{
    return glm::mix(a, b, t);
}

/// Interpolates between angles and wraps around 360 degrees
float LerpAngle(float a, float b, float t)
{
    float diff = fmodf(b - a + 540.f, 360.f) - 180.f;
    return a + diff * t;
}

AABB::AABB() 
{ 
    Reset(); 
}

/// Resets the AABB to an empty state
void AABB::Reset()
{
    min = Vector3f(std::numeric_limits<float>::max());
    max = Vector3f(std::numeric_limits<float>::lowest());
}

bool AABB::IsEmpty() const
{
    return (min.x > max.x) || (min.y > max.y) || (min.z > max.z);
}

/// Expands the AABB to include the specified point
void AABB::Expand(const Vector3f& point)
{
    min = glm::min(min, point);
    max = glm::max(max, point);
}

void AABB::Expand(const AABB& other)
{
    if (!other.IsEmpty())
    {
        if (IsEmpty())
        {
            *this = other;
        }
        else
        {
            min = glm::min(min, other.min);
            max = glm::max(max, other.max);
        }
    }
}

/// Transforms this AABB by a transformation matrix and returns the result
AABB AABB::Transform(const Matrix4f& modelMatrix) const
{
    AABB result;

    // Define 8 corners of the original box
    Vector3f corners[8] = {
        {min.x, min.y, min.z}, {max.x, min.y, min.z},
        {min.x, max.y, min.z}, {max.x, max.y, min.z},
        {min.x, min.y, max.z}, {max.x, min.y, max.z},
        {min.x, max.y, max.z}, {max.x, max.y, max.z}
    };

    // Transform each corner and expand the result AABB
    for (int i = 0; i < 8; ++i)
    {
        Vector3f transformed = Vector3f(modelMatrix * Vector4f(corners[i], 1.0f));
        result.Expand(transformed);
    }

    return result;
}