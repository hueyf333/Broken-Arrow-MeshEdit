// File: MeshEditor/src/scene/Scene.cpp
#include "scene/Scene.h"
#include <algorithm>

namespace MeshEditor
{
    Entity& Scene::CreateEntity(const std::string& name)
    {
        mEntities.push_back(std::make_unique<Entity>(name));
        return *mEntities.back();
    }

    void Scene::RemoveEntity(uint64_t id)
    {
        mEntities.erase(
            std::remove_if(mEntities.begin(), mEntities.end(), [id](const std::unique_ptr<Entity>& entity)
                           { return entity->Id() == id; }),
            mEntities.end());
    }

    Entity* Scene::FindEntity(uint64_t id)
    {
        for (auto& entity : mEntities)
        {
            if (entity->Id() == id)
            {
                return entity.get();
            }
        }
        return nullptr;
    }

    const std::vector<std::unique_ptr<Entity>>& Scene::Entities() const
    {
        return mEntities;
    }
}
