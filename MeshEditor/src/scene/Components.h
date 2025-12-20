// File: MeshEditor/src/scene/Components.h
#pragma once

#include <memory>
#include <string>
#include <glm/glm.hpp>

namespace mesh
{
class RenderMesh;

struct TransformComponent
{
    glm::vec3 position{0.0f, 0.0f, 0.0f};
    glm::vec3 rotation{0.0f, 0.0f, 0.0f};
    glm::vec3 scale{1.0f, 1.0f, 1.0f};
};

struct MeshComponent
{
    std::shared_ptr<RenderMesh> mesh;
};

struct MaterialComponent
{
    glm::vec4 baseColor{0.8f, 0.8f, 0.8f, 1.0f};
    std::string baseColorTexture;
};
}
