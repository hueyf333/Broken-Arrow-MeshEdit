// File: MeshEditor/src/tools/Tool_Select.cpp
#include "Tool_Select.h"

namespace mesh
{
Tool_Select::Tool_Select(Scene& scene, Selection& selection, ViewportWidget& viewport)
    : scene_(scene), selection_(selection), viewport_(viewport)
{
}
}
