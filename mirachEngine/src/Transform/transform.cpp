#include "transform.hpp"

Transform::Transform()
    : 
    m_yAxisUp(true)
{
    // Initialize model matrix to identity
    m_modelMatrix = Matrix4f(1.0f);

    // Set default position, scale, and rotation
    m_position = { 0.0f, 0.0f, 0.0f };
    m_rotation = Quaternion{ 1.0f, 0.0f, 0.0f, 0.0f }; // Identity quaternion
    m_scale = { 1.0f, 1.0f, 1.0f };
}


Matrix4f Transform::GetModelMatrix() const
{
    // Return the cached model matrix
    return m_modelMatrix;
}


void Transform::SetPosition(float x, float y, float z)
{
    // Update position and recompute model matrix
    m_position = { x, y, z };
    UpdateModelMatrix();
}

void Transform::SetPosition(const Vector3f& position)
{
    m_position = position;
    UpdateModelMatrix();
}


void Transform::SetRotation(const Quaternion& q)
{
    // Normalize quaternion to prevent drift and store it
    m_rotation = Normalize(q);
    UpdateModelMatrix();
}

void Transform::SetRotation(float x, float y, float z)
{
    // Clamp each angle into [0, 360)
    x = ResetRotation(x);
    y = ResetRotation(y);
    z = ResetRotation(z);

    // Convert Euler angles to quaternion
    m_rotation = EulerDegreesToQuaternion(Vector3f(x, y, z));

    UpdateModelMatrix();
}

float Transform::ResetRotation(float axis)
{
    // Wrap value into the range [0, 360)
    axis = std::fmodf(axis, 360.0f);
    if (axis < 0.0f)
    {
        axis += 360.0f;
    }

    return axis;
}

void Transform::SetScale(float x, float y, float z)
{
    // Update scale vector and rebuild matrix
    m_scale = { x, y, z };
    UpdateModelMatrix();
}

void Transform::SetScale(const Vector3f& scale)
{
    m_scale = scale;
    UpdateModelMatrix();
}

void Transform::SetYAxisUp(bool up)
{
    // Used to control coordinate system behavior externally
    m_yAxisUp = up;
}

Vector3f Transform::GetPosition() const
{
    // Return the current position
    return m_position;
}

Vector3f Transform::GetRotation() const
{
    // Convert and return rotation in Euler angles (degrees)
    return QuaternionToEulerDegrees(m_rotation);
}

Quaternion Transform::GetQuaternionRotation() const
{
    return m_rotation;
}

Vector3f Transform::GetScale() const
{
    // Return the current scale
    return m_scale;
}

bool Transform::IsYAxisUp() const
{
    // Indicates whether Y or Z is considered the 'up' direction
    return m_yAxisUp;
}

void Transform::UpdateModelMatrix()
{
    // Start from identity matrix
    m_modelMatrix = Matrix4f(1.0f);

    // Apply translation
    m_modelMatrix = Translate(m_modelMatrix, m_position);

    // Convert quaternion to rotation matrix and apply
    Matrix4f rotationMatrix = ToMat4(m_rotation);
    m_modelMatrix *= rotationMatrix;

    // Apply scaling
    m_modelMatrix = Scale(m_modelMatrix, m_scale);
}
