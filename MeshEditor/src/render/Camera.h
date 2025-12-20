// File: MeshEditor/src/render/Camera.h
#pragma once

#include <glm/glm.hpp>

namespace MeshEditor
{
    class Camera
    {
    public:
        void SetViewport(float width, float height);
        void SetOrbit(float yaw, float pitch, float distance);
        void SetTarget(const glm::vec3& target);
        void Dolly(float delta);
        void Pan(const glm::vec2& delta);
        void Orbit(const glm::vec2& delta);

        glm::mat4 View() const;
        glm::mat4 Projection() const;
        glm::mat4 ViewProjection() const;

        glm::vec3 Position() const;
        glm::vec3 Target() const;

    private:
        float mWidth = 1280.0f;
        float mHeight = 720.0f;
        float mYaw = 0.7f;
        float mPitch = 0.6f;
        float mDistance = 8.0f;
        glm::vec3 mTarget {0.0f};
    };
}
