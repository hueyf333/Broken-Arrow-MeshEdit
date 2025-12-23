#include "Core/Raycast.hpp"
#include <limits>
#include <cmath>

namespace Raycast {

bool intersectTriangle(
    const glm::vec3& rayOrigin,
    const glm::vec3& rayDir,
    const glm::vec3& v0,
    const glm::vec3& v1,
    const glm::vec3& v2,
    float& t
) {
    // Möller–Trumbore intersection algorithm
    const float EPSILON = 0.0000001f;
    
    glm::vec3 edge1 = v1 - v0;
    glm::vec3 edge2 = v2 - v0;
    glm::vec3 h = glm::cross(rayDir, edge2);
    float a = glm::dot(edge1, h);
    
    if (std::abs(a) < EPSILON) {
        return false; // Ray is parallel to triangle
    }
    
    float f = 1.0f / a;
    glm::vec3 s = rayOrigin - v0;
    float u = f * glm::dot(s, h);
    
    if (u < 0.0f || u > 1.0f) {
        return false;
    }
    
    glm::vec3 q = glm::cross(s, edge1);
    float v = f * glm::dot(rayDir, q);
    
    if (v < 0.0f || u + v > 1.0f) {
        return false;
    }
    
    t = f * glm::dot(edge2, q);
    
    if (t > EPSILON) {
        return true;
    }
    
    return false;
}

std::optional<RayHit> castRay(
    const glm::vec3& rayOrigin,
    const glm::vec3& rayDir,
    const Mesh& mesh,
    const glm::mat4& transform,
    int objectIndex
) {
    float closestT = std::numeric_limits<float>::max();
    int closestTriangle = -1;
    
    for (size_t i = 0; i < mesh.indices.size(); i += 3) {
        glm::vec3 v0 = glm::vec3(transform * glm::vec4(mesh.vertices[mesh.indices[i]], 1.0f));
        glm::vec3 v1 = glm::vec3(transform * glm::vec4(mesh.vertices[mesh.indices[i + 1]], 1.0f));
        glm::vec3 v2 = glm::vec3(transform * glm::vec4(mesh.vertices[mesh.indices[i + 2]], 1.0f));
        
        float t;
        if (intersectTriangle(rayOrigin, rayDir, v0, v1, v2, t)) {
            if (t < closestT) {
                closestT = t;
                closestTriangle = static_cast<int>(i / 3);
            }
        }
    }
    
    if (closestTriangle >= 0) {
        RayHit hit;
        hit.objectIndex = objectIndex;
        hit.triangleIndex = closestTriangle;
        hit.vertexIndex = mesh.indices[closestTriangle * 3]; // First vertex of triangle
        hit.hitPoint = rayOrigin + rayDir * closestT;
        hit.distance = closestT;
        return hit;
    }
    
    return std::nullopt;
}

int findClosestVertex(
    const glm::vec3& point,
    const Mesh& mesh,
    const glm::mat4& transform,
    float maxDistance
) {
    float closestDistSq = maxDistance * maxDistance;
    int closestVertex = -1;
    
    for (size_t i = 0; i < mesh.vertices.size(); ++i) {
        glm::vec3 v = glm::vec3(transform * glm::vec4(mesh.vertices[i], 1.0f));
        glm::vec3 diff = v - point;
        float distSq = glm::dot(diff, diff);
        
        if (distSq < closestDistSq) {
            closestDistSq = distSq;
            closestVertex = static_cast<int>(i);
        }
    }
    
    return closestVertex;
}

} // namespace Raycast
