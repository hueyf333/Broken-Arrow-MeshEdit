// File: MeshEditor/src/scene/Selection.cpp
#include "Selection.h"

namespace mesh
{
void Selection::Set(EntityId id)
{
    selected_ = id;
}

void Selection::Clear()
{
    selected_.reset();
}

bool Selection::HasSelection() const
{
    return selected_.has_value();
}

std::optional<EntityId> Selection::Selected() const
{
    return selected_;
}
}
