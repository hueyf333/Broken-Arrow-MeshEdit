// File: MeshEditor/src/scene/Components.h
#pragma once

#include <vector>
#include <string>
#include <glm/glm.hpp>
#include "engine/Math.h"

namespace MeshEditor
{
struct Vertex
{
    glm::vec3 position;
    glm::vec3 normal;
    glm::vec2 uv;
};

struct MeshComponent
{
    std::vector<Vertex> vertices;
    std::vector<uint32_t> indices;
    AABB bounds{{-0.5f, -0.5f, -0.5f}, {0.5f, 0.5f, 0.5f}};
};

struct MaterialComponent
{
    glm::vec4 baseColor{0.8f, 0.8f, 0.8f, 1.0f};
    std::string baseColorTexture;
};
}
