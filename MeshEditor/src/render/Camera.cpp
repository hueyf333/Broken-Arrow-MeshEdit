// File: MeshEditor/src/render/Camera.cpp
#include "Camera.h"

#include <glm/gtc/matrix_transform.hpp>

namespace mesh
{
void Camera::SetViewport(float width, float height)
{
    aspect_ = (height > 0.0f) ? width / height : 1.0f;
}

void Camera::Orbit(float dx, float dy)
{
    yaw_ += dx * 0.005f;
    pitch_ += dy * 0.005f;
    pitch_ = glm::clamp(pitch_, -1.5f, 1.5f);
}

void Camera::Pan(float dx, float dy)
{
    glm::vec3 right = glm::normalize(glm::cross(Position() - target_, glm::vec3(0.0f, 1.0f, 0.0f)));
    glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);
    target_ += (-right * dx + up * dy) * 0.01f * distance_;
}

void Camera::Dolly(float delta)
{
    distance_ += delta * 0.01f;
    if (distance_ < 0.2f)
    {
        distance_ = 0.2f;
    }
}

void Camera::Frame(const glm::vec3& center, float radius)
{
    target_ = center;
    distance_ = radius * 2.5f + 0.1f;
}

glm::mat4 Camera::ViewMatrix() const
{
    glm::vec3 position = Position();
    return glm::lookAt(position, target_, glm::vec3(0.0f, 1.0f, 0.0f));
}

glm::mat4 Camera::ProjectionMatrix() const
{
    return glm::perspective(glm::radians(60.0f), aspect_, 0.1f, 500.0f);
}

glm::vec3 Camera::Position() const
{
    glm::vec3 dir;
    dir.x = cosf(pitch_) * cosf(yaw_);
    dir.y = sinf(pitch_);
    dir.z = cosf(pitch_) * sinf(yaw_);
    return target_ + (-dir) * distance_;
}
}
