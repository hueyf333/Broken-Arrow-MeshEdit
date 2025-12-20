// File: MeshEditor/src/engine/Math.h
#pragma once

#include <glm/glm.hpp>

namespace MeshEditor
{
struct Ray
{
    glm::vec3 origin;
    glm::vec3 direction;
};

struct AABB
{
    glm::vec3 min;
    glm::vec3 max;
};

bool IntersectRayAABB(const Ray& ray, const AABB& box, float& t);
}
