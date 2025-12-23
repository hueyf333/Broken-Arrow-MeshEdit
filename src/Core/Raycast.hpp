#pragma once
#include "Core/Mesh.hpp"
#include "Core/Camera.hpp"
#include <glm/glm.hpp>
#include <optional>

struct RayHit {
    int objectIndex;
    int triangleIndex;
    int vertexIndex;
    glm::vec3 hitPoint;
    float distance;
};

namespace Raycast {
    // Ray-triangle intersection
    bool intersectTriangle(
        const glm::vec3& rayOrigin,
        const glm::vec3& rayDir,
        const glm::vec3& v0,
        const glm::vec3& v1,
        const glm::vec3& v2,
        float& t
    );
    
    // Cast ray against mesh
    std::optional<RayHit> castRay(
        const glm::vec3& rayOrigin,
        const glm::vec3& rayDir,
        const Mesh& mesh,
        const glm::mat4& transform,
        int objectIndex
    );
    
    // Find closest vertex to point
    int findClosestVertex(
        const glm::vec3& point,
        const Mesh& mesh,
        const glm::mat4& transform,
        float maxDistance
    );
}
