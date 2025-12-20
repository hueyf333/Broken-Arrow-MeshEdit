// File: MeshEditor/src/engine/Core.h
#pragma once

#include <cstdint>
#include <string>
#include <vector>

namespace mesh
{
using EntityId = std::uint64_t;

struct Aabb
{
    float min[3]{};
    float max[3]{};
};
}
