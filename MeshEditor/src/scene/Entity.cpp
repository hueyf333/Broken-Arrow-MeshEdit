// File: MeshEditor/src/scene/Entity.cpp
#include "Entity.h"

namespace MeshEditor
{
Entity::Entity(int id, const std::string& name)
    : m_id(id)
    , m_name(name)
{
}

AABB Entity::GetBounds() const
{
    return m_mesh.bounds;
}
}
