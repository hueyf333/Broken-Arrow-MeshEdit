// File: MeshEditor/src/scene/Scene.h
#pragma once

#include <memory>
#include <vector>
#include "Entity.h"

namespace mesh
{
class Scene
{
public:
    Entity& CreateEntity(const std::string& name);
    void DestroyEntity(EntityId id);

    std::vector<std::unique_ptr<Entity>>& Entities();
    const std::vector<std::unique_ptr<Entity>>& Entities() const;

    Entity* Find(EntityId id);

private:
    EntityId nextId_ = 1;
    std::vector<std::unique_ptr<Entity>> entities_;
};
}
