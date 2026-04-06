#include "camera.hpp"

#include <iostream>
#include <reactphysics3d/reactphysics3d.h>

Camera::Camera(const Vector3f& localPosition)
    :
    m_worldUp(0.0f, 1.0f, 0.0f),   // Set world up direction
    m_yaw(-90.0f),                // Default yaw facing -Z
    m_pitch(0.0f),                // Level pitch
    m_enabled(true),               // Camera enabled by default
    m_localPosition(localPosition)
{
    UpdateVectors();             // Initialize direction vectors
}

void Camera::SetProjectionMatrix(float fov, float aspect, float near, float far)
{
    // Create a perspective projection matrix using field of view and aspect ratio
    m_projectionMatrix = Perspective(ToRadians(fov), aspect, near, far);
}

Matrix4f Camera::GetViewMatrix(const Vector3f& position) const
{
    Vector3f worldPosition = position + m_localPosition;
    // Return view matrix from position and current direction
    return LookAt(worldPosition, worldPosition + m_frontVector, m_upVector);
}

Matrix4f Camera::GetProjectionMatrix() const
{
    return m_projectionMatrix;
}

void Camera::Look(float deltaX, float deltaY, float lookSpeed)
{
    // Adjust yaw and pitch based on mouse movement and sensitivity
    m_yaw += deltaX * lookSpeed;
    m_pitch -= deltaY * lookSpeed;

    // Clamp vertical rotation to avoid gimbal lock
    m_pitch = Clamp(m_pitch, -89.0f, 89.0f);

    // Update direction vectors after rotation
    UpdateVectors();
}

void Camera::UpdateVectors()
{
    // Convert yaw/pitch angles to a directional front vector
    Vector3f front;
    front.x = cos(ToRadians(m_yaw)) * cos(ToRadians(m_pitch));
    front.y = sin(ToRadians(m_pitch));
    front.z = sin(ToRadians(m_yaw)) * cos(ToRadians(m_pitch));
    m_frontVector = Normalize(front);

    // Calculate right and up vectors from front and world up
    m_rightVector = Normalize(Cross(m_frontVector, m_worldUp));
    m_upVector = Normalize(Cross(m_rightVector, m_frontVector));
}

bool Camera::IsBoxInFrustum(const Vector3f& min, const Vector3f& max) const
{
    // Check if AABB is inside all 6 frustum planes
    for (int i = 0; i < 6; ++i)
    {
        const Vector4f& plane = m_frustumPlanes[i];

        // Use positive vertex based on the plane's normal direction
        Vector3f positive = min;
        if (plane.x >= 0)
        {
            positive.x = max.x;
        }
        if (plane.y >= 0)
        {
            positive.y = max.y;
        }
        if (plane.z >= 0)
        {
            positive.z = max.z;
        }

        // If positive vertex is outside the plane, box is not visible
        if (Dot(Vector3f(plane), positive) + plane.w < 0)
        {
            return false;
        }
    }
    return true; // Box is at least partially in frustum
}

void Camera::Enable(bool enabled)
{
    m_enabled = enabled;
}

bool Camera::IsEnabled() const
{
    return m_enabled;
}

void Camera::UpdateFrustumPlanes(const Vector3f& position)
{
    // Compute combined View-Projection matrix
    Matrix4f VP = m_projectionMatrix * GetViewMatrix(position);

    // Extract the six frustum planes from the VP matrix (left, right, bottom, top, near, far)
    m_frustumPlanes[0] = Vector4f(VP[0][3] + VP[0][0], VP[1][3] + VP[1][0], VP[2][3] + VP[2][0], VP[3][3] + VP[3][0]); // Left
    m_frustumPlanes[1] = Vector4f(VP[0][3] - VP[0][0], VP[1][3] - VP[1][0], VP[2][3] - VP[2][0], VP[3][3] - VP[3][0]); // Right
    m_frustumPlanes[2] = Vector4f(VP[0][3] + VP[0][1], VP[1][3] + VP[1][1], VP[2][3] + VP[2][1], VP[3][3] + VP[3][1]); // Bottom
    m_frustumPlanes[3] = Vector4f(VP[0][3] - VP[0][1], VP[1][3] - VP[1][1], VP[2][3] - VP[2][1], VP[3][3] - VP[3][1]); // Top
    m_frustumPlanes[4] = Vector4f(VP[0][3] + VP[0][2], VP[1][3] + VP[1][2], VP[2][3] + VP[2][2], VP[3][3] + VP[3][2]); // Near
    m_frustumPlanes[5] = Vector4f(VP[0][3] - VP[0][2], VP[1][3] - VP[1][2], VP[2][3] - VP[2][2], VP[3][3] - VP[3][2]); // Far

    // Normalize all plane equations
    for (int i = 0; i < 6; i++)
    {
        float length = Length(Vector4f(m_frustumPlanes[i]));
        m_frustumPlanes[i] /= length;
    }
}

Vector3f Camera::GetFrontVector() const
{
    return m_frontVector;
}

Vector3f Camera::GetRightVector() const
{
    return m_rightVector;
}

Vector3f Camera::GetLocalPosition() const
{
    return m_localPosition;
}
