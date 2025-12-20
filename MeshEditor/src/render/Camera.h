// File: MeshEditor/src/render/Camera.h
#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace MeshEditor
{
class Camera
{
public:
    Camera();

    void Orbit(float dx, float dy);
    void Pan(float dx, float dy);
    void Dolly(float delta);

    glm::mat4 GetView() const;
    glm::mat4 GetProjection(float aspect) const;

    glm::vec3 position;
    glm::vec3 target;
    glm::vec3 up;
    float distance;
};
}
