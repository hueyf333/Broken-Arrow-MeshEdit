// File: MeshEditor/src/render/Camera.cpp
#include "render/Camera.h"
#include <glm/gtc/matrix_transform.hpp>
#include <algorithm>
#include <cmath>

namespace MeshEditor
{
    void Camera::SetViewport(float width, float height)
    {
        mWidth = std::max(1.0f, width);
        mHeight = std::max(1.0f, height);
    }

    void Camera::SetOrbit(float yaw, float pitch, float distance)
    {
        mYaw = yaw;
        mPitch = pitch;
        mDistance = std::max(0.1f, distance);
    }

    void Camera::SetTarget(const glm::vec3& target)
    {
        mTarget = target;
    }

    void Camera::Dolly(float delta)
    {
        mDistance = std::max(0.2f, mDistance + delta);
    }

    void Camera::Pan(const glm::vec2& delta)
    {
        glm::vec3 right = glm::normalize(glm::cross(Position() - mTarget, glm::vec3(0, 1, 0)));
        glm::vec3 up = glm::normalize(glm::cross(right, Position() - mTarget));
        mTarget += (-right * delta.x + up * delta.y);
    }

    void Camera::Orbit(const glm::vec2& delta)
    {
        mYaw += delta.x;
        mPitch = std::clamp(mPitch + delta.y, -1.5f, 1.5f);
    }

    glm::vec3 Camera::Position() const
    {
        float x = mDistance * cosf(mPitch) * cosf(mYaw);
        float y = mDistance * sinf(mPitch);
        float z = mDistance * cosf(mPitch) * sinf(mYaw);
        return mTarget + glm::vec3(x, y, z);
    }

    glm::vec3 Camera::Target() const
    {
        return mTarget;
    }

    glm::mat4 Camera::View() const
    {
        return glm::lookAt(Position(), mTarget, glm::vec3(0, 1, 0));
    }

    glm::mat4 Camera::Projection() const
    {
        float aspect = mWidth / mHeight;
        return glm::perspective(glm::radians(60.0f), aspect, 0.1f, 1000.0f);
    }

    glm::mat4 Camera::ViewProjection() const
    {
        return Projection() * View();
    }
}
