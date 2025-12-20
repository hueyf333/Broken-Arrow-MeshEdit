#pragma once

#include "Mesh.h"

namespace Primitives {
    Mesh MakeCube(float size = 1.0f);
    Mesh MakePlane(float size = 1.0f);
    Mesh MakeSphere(float radius = 1.0f, int slices = 24, int stacks = 16);
    Mesh MakeCylinder(float radius = 0.5f, float height = 1.0f, int slices = 24);
}
