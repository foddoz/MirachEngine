#ifndef CAMERA_HPP
#define CAMERA_HPP

#include "../Math/math.hpp"

/**
 * @class Camera
 * @brief A 3D camera that handles projection, view matrix generation, frustum culling, and orientation logic.
 *
 * Used in the Mirach engine to compute view and projection matrices,
 * manage orientation using pitch and yaw, and determine whether objects are visible within the view frustum.
 */
class Camera
{
public:
    /**
     * @brief Constructs a camera with default orientation and projection parameters.
     */
    Camera(const Vector3f& localPosition = Vector3f(0.f));

    /**
     * @brief Sets the perspective projection matrix using a field of view, aspect ratio, and near/far planes.
     * @param fov Field of view in radians.
     * @param aspect Aspect ratio (width divided by height).
     * @param near Near clipping plane distance.
     * @param far Far clipping plane distance.
     */
    void SetProjectionMatrix(float fov, float aspect, float near, float far);

    /**
     * @brief Calculates and retrieves the current view matrix based on a given position.
     * @param position The world-space position of the camera.
     * @return View matrix (Matrix4f).
     */
    Matrix4f GetViewMatrix(const Vector3f& position) const;

    /**
     * @brief Retrieves the current projection matrix.
     * @return Projection matrix (Matrix4f).
     */
    Matrix4f GetProjectionMatrix() const;

    /**
     * @brief Applies mouse input to rotate the camera using yaw and pitch.
     * @param deltaX Change in mouse X position.
     * @param deltaY Change in mouse Y position.
     * @param lookSpeed Sensitivity multiplier for look speed.
     */
    void Look(float deltaX, float deltaY, float lookSpeed);

    /**
     * @brief Tests whether a given axis-aligned bounding box (AABB) is inside the camera's view frustum.
     * @param min Minimum corner of the AABB (Vector3f).
     * @param max Maximum corner of the AABB (Vector3f).
     * @return True if the box is at least partially inside the frustum; false otherwise.
     */
    bool IsBoxInFrustum(const Vector3f& min, const Vector3f& max) const;

    /**
     * @brief Enables or disables the camera.
     * @param enabled True to enable the camera, false to disable.
     */
    void Enable(bool enabled);

    /**
     * @brief Checks if the camera is currently enabled.
     * @return True if enabled, false otherwise.
     */
    bool IsEnabled() const;

    /**
     * @brief Updates the six frustum planes based on the camera's view-projection matrix and position.
     * @param position The world-space position of the camera.
     */
    void UpdateFrustumPlanes(const Vector3f& position);

    /**
     * @brief Retrieves the camera's forward (front) direction vector.
     * @return Normalized front vector (Vector3f).
     */
    Vector3f GetFrontVector() const;

    /**
     * @brief Retrieves the camera's right direction vector.
     * @return Normalized right vector (Vector3f).
     */
    Vector3f GetRightVector() const;

    Vector3f GetLocalPosition() const;

private:
    /**
     * @brief Updates the camera's direction vectors (front, right, up) based on its current yaw and pitch.
     */
    void UpdateVectors();

    Matrix4f m_projectionMatrix;        ///< Perspective projection matrix.

    Vector3f m_localPosition;

    Vector3f m_frontVector;             ///< Normalized forward direction vector.
    Vector3f m_rightVector;             ///< Normalized right direction vector.
    Vector3f m_upVector;                ///< Normalized up direction vector.
    Vector3f m_worldUp;                 ///< World-space up vector, usually (0,1,0).

    float m_yaw;                        ///< Horizontal angle in degrees (yaw rotation).
    float m_pitch;                      ///< Vertical angle in degrees (pitch rotation).

    Vector4f m_frustumPlanes[6];        ///< View frustum planes: left, right, bottom, top, near, far.

    bool m_enabled;                     ///< Whether the camera is currently active or not.
};

#endif
