// File: MeshEditor/src/engine/Math.cpp
#include "Math.h"

#include <algorithm>

namespace mesh
{
bool RayIntersectsAabb(const Ray& ray, const glm::vec3& min, const glm::vec3& max, float& tOut)
{
    float tMin = 0.0f;
    float tMax = 1e9f;
    for (int i = 0; i < 3; ++i)
    {
        if (glm::abs(ray.direction[i]) < 1e-6f)
        {
            if (ray.origin[i] < min[i] || ray.origin[i] > max[i])
            {
                return false;
            }
        }
        else
        {
            float invD = 1.0f / ray.direction[i];
            float t0 = (min[i] - ray.origin[i]) * invD;
            float t1 = (max[i] - ray.origin[i]) * invD;
            if (t0 > t1)
            {
                std::swap(t0, t1);
            }
            tMin = t0 > tMin ? t0 : tMin;
            tMax = t1 < tMax ? t1 : tMax;
            if (tMax < tMin)
            {
                return false;
            }
        }
    }
    tOut = tMin;
    return true;
}
}
