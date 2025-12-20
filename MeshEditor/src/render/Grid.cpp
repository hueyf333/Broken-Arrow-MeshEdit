// File: MeshEditor/src/render/Grid.cpp
#include "Grid.h"

namespace MeshEditor
{
void Grid::Build(int lines, float spacing)
{
    vertices.clear();
    int half = lines / 2;
    for (int i = -half; i <= half; ++i)
    {
        float v = i * spacing;
        vertices.push_back(glm::vec3(v, 0.0f, -half * spacing));
        vertices.push_back(glm::vec3(v, 0.0f, half * spacing));
        vertices.push_back(glm::vec3(-half * spacing, 0.0f, v));
        vertices.push_back(glm::vec3(half * spacing, 0.0f, v));
    }
}
}
