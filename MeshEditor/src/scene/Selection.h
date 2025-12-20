// File: MeshEditor/src/scene/Selection.h
#pragma once

#include <optional>
#include "engine/Core.h"

namespace mesh
{
class Selection
{
public:
    void Set(EntityId id);
    void Clear();
    bool HasSelection() const;
    std::optional<EntityId> Selected() const;

private:
    std::optional<EntityId> selected_;
};
}
