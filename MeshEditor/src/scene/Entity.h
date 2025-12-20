// File: MeshEditor/src/scene/Entity.h
#pragma once

#include <string>
#include "engine/Core.h"
#include "scene/Components.h"

namespace mesh
{
class Entity
{
public:
    Entity(EntityId id, std::string name);

    EntityId Id() const;
    const std::string& Name() const;
    void SetName(const std::string& name);

    TransformComponent& Transform();
    const TransformComponent& Transform() const;

    MeshComponent& Mesh();
    const MeshComponent& Mesh() const;

    MaterialComponent& Material();
    const MaterialComponent& Material() const;

private:
    EntityId id_;
    std::string name_;
    TransformComponent transform_{};
    MeshComponent mesh_{};
    MaterialComponent material_{};
};
}
