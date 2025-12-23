#include "Core/Camera.hpp"
#include <algorithm>

Camera::Camera()
    : m_position(5.0f, 5.0f, 5.0f)
    , m_target(0.0f, 0.0f, 0.0f)
    , m_up(0.0f, 1.0f, 0.0f)
    , m_distance(8.66f)
    , m_yaw(-45.0f)
    , m_pitch(35.0f)
    , m_fov(45.0f)
    , m_aspect(16.0f / 9.0f)
    , m_near(0.1f)
    , m_far(1000.0f)
    , m_orthoSize(10.0f)
    , m_mode(CameraMode::Perspective)
{
    updatePosition();
    updateProjection();
}

glm::mat4 Camera::getViewMatrix() const {
    return glm::lookAt(m_position, m_target, m_up);
}

void Camera::orbit(float deltaYaw, float deltaPitch) {
    m_yaw += deltaYaw;
    m_pitch += deltaPitch;
    m_pitch = std::clamp(m_pitch, -89.0f, 89.0f);
    updatePosition();
}

void Camera::pan(float deltaX, float deltaY) {
    glm::vec3 forward = glm::normalize(m_target - m_position);
    glm::vec3 right = glm::normalize(glm::cross(forward, m_up));
    glm::vec3 up = glm::cross(right, forward);
    
    float panSpeed = m_distance * 0.001f;
    m_target += (right * deltaX + up * deltaY) * panSpeed;
    updatePosition();
}

void Camera::zoom(float delta) {
    m_distance -= delta * 0.1f;
    m_distance = std::max(0.1f, m_distance);
    
    if (m_mode == CameraMode::Orthographic) {
        m_orthoSize = m_distance;
        updateProjection();
    }
    
    updatePosition();
}

void Camera::frameTarget(const glm::vec3& target, float radius) {
    m_target = target;
    m_distance = radius * 2.5f;
    updatePosition();
}

glm::vec3 Camera::getRayDirection(float ndcX, float ndcY) const {
    glm::mat4 invProj = glm::inverse(m_projection);
    glm::mat4 invView = glm::inverse(getViewMatrix());
    
    glm::vec4 rayClip(ndcX, ndcY, -1.0f, 1.0f);
    glm::vec4 rayEye = invProj * rayClip;
    rayEye = glm::vec4(rayEye.x, rayEye.y, -1.0f, 0.0f);
    glm::vec4 rayWorld = invView * rayEye;
    
    return glm::normalize(glm::vec3(rayWorld));
}

void Camera::updatePosition() {
    float yawRad = glm::radians(m_yaw);
    float pitchRad = glm::radians(m_pitch);
    
    m_position.x = m_target.x + m_distance * cos(pitchRad) * cos(yawRad);
    m_position.y = m_target.y + m_distance * sin(pitchRad);
    m_position.z = m_target.z + m_distance * cos(pitchRad) * sin(yawRad);
}

void Camera::updateProjection() {
    if (m_mode == CameraMode::Perspective) {
        m_projection = glm::perspective(glm::radians(m_fov), m_aspect, m_near, m_far);
    } else {
        // Clamp to prevent degenerate projections
        float clampedSize = std::max(0.1f, m_orthoSize);
        float halfWidth = clampedSize * m_aspect;
        float halfHeight = clampedSize;
        m_projection = glm::ortho(-halfWidth, halfWidth, -halfHeight, halfHeight, m_near, m_far);
    }
}
