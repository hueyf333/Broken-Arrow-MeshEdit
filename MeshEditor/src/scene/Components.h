// File: MeshEditor/src/scene/Components.h
#pragma once

#include <glm/glm.hpp>
#include <string>
#include <vector>

namespace MeshEditor
{
    struct Transform
    {
        glm::vec3 position {0.0f};
        glm::vec3 rotation {0.0f};
        glm::vec3 scale {1.0f};
    };

    struct MeshData
    {
        std::vector<glm::vec3> positions;
        std::vector<glm::vec3> normals;
        std::vector<glm::vec2> uvs;
        std::vector<uint32_t> indices;
    };

    struct Material
    {
        glm::vec4 baseColor {1.0f, 1.0f, 1.0f, 1.0f};
        std::string baseColorTexturePath;
    };
}
