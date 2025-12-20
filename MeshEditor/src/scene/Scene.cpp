// File: MeshEditor/src/scene/Scene.cpp
#include "Scene.h"

#include <algorithm>

namespace mesh
{
Entity& Scene::CreateEntity(const std::string& name)
{
    auto entity = std::make_unique<Entity>(nextId_++, name);
    entities_.push_back(std::move(entity));
    return *entities_.back();
}

void Scene::DestroyEntity(EntityId id)
{
    entities_.erase(std::remove_if(entities_.begin(), entities_.end(),
                                  [id](const std::unique_ptr<Entity>& entity)
                                  {
                                      return entity->Id() == id;
                                  }),
                    entities_.end());
}

std::vector<std::unique_ptr<Entity>>& Scene::Entities()
{
    return entities_;
}

const std::vector<std::unique_ptr<Entity>>& Scene::Entities() const
{
    return entities_;
}

Entity* Scene::Find(EntityId id)
{
    for (auto& entity : entities_)
    {
        if (entity->Id() == id)
        {
            return entity.get();
        }
    }
    return nullptr;
}
}
