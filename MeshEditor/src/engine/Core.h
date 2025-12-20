// File: MeshEditor/src/engine/Core.h
#pragma once

#include <string>
#include <vector>
#include <memory>
#include <glm/glm.hpp>

namespace MeshEditor
{
struct Transform
{
    glm::vec3 position{0.0f};
    glm::vec3 rotation{0.0f};
    glm::vec3 scale{1.0f};
};
}
