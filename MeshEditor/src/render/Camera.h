// File: MeshEditor/src/render/Camera.h
#pragma once

#include <glm/glm.hpp>

namespace mesh
{
class Camera
{
public:
    void SetViewport(float width, float height);
    void Orbit(float dx, float dy);
    void Pan(float dx, float dy);
    void Dolly(float delta);
    void Frame(const glm::vec3& center, float radius);

    glm::mat4 ViewMatrix() const;
    glm::mat4 ProjectionMatrix() const;
    glm::vec3 Position() const;

private:
    float yaw_ = 0.0f;
    float pitch_ = 0.0f;
    float distance_ = 6.0f;
    glm::vec3 target_{0.0f, 0.0f, 0.0f};
    float aspect_ = 1.0f;
};
}
