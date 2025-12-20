// File: MeshEditor/src/render/Camera.cpp
#include "Camera.h"

namespace MeshEditor
{
Camera::Camera()
    : position(0.0f, 2.0f, 6.0f)
    , target(0.0f, 0.0f, 0.0f)
    , up(0.0f, 1.0f, 0.0f)
    , distance(6.0f)
{
}

void Camera::Orbit(float dx, float dy)
{
    position.x += dx;
    position.y += dy;
}

void Camera::Pan(float dx, float dy)
{
    target.x += dx;
    target.y += dy;
}

void Camera::Dolly(float delta)
{
    position.z += delta;
}

glm::mat4 Camera::GetView() const
{
    return glm::lookAt(position, target, up);
}

glm::mat4 Camera::GetProjection(float aspect) const
{
    return glm::perspective(glm::radians(60.0f), aspect, 0.1f, 1000.0f);
}
}
