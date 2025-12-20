// File: MeshEditor/src/engine/Math.h
#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace mesh
{
struct Ray
{
    glm::vec3 origin{};
    glm::vec3 direction{};
};

bool RayIntersectsAabb(const Ray& ray, const glm::vec3& min, const glm::vec3& max, float& tOut);
}
