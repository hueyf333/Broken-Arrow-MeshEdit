// File: MeshEditor/src/scene/Entity.cpp
#include "Entity.h"

namespace mesh
{
Entity::Entity(EntityId id, std::string name)
    : id_(id), name_(std::move(name))
{
}

EntityId Entity::Id() const
{
    return id_;
}

const std::string& Entity::Name() const
{
    return name_;
}

void Entity::SetName(const std::string& name)
{
    name_ = name;
}

TransformComponent& Entity::Transform()
{
    return transform_;
}

const TransformComponent& Entity::Transform() const
{
    return transform_;
}

MeshComponent& Entity::Mesh()
{
    return mesh_;
}

const MeshComponent& Entity::Mesh() const
{
    return mesh_;
}

MaterialComponent& Entity::Material()
{
    return material_;
}

const MaterialComponent& Entity::Material() const
{
    return material_;
}
}
