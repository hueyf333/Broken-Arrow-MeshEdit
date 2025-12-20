// File: MeshEditor/src/render/Grid.h
#pragma once

#include <vector>
#include <glm/glm.hpp>

namespace MeshEditor
{
class Grid
{
public:
    std::vector<glm::vec3> vertices;
    void Build(int lines = 20, float spacing = 1.0f);
};
}
