// File: MeshEditor/src/scene/Scene.h
#pragma once

#include "scene/Entity.h"
#include <memory>
#include <vector>

namespace MeshEditor
{
    class Scene
    {
    public:
        Entity& CreateEntity(const std::string& name);
        void RemoveEntity(uint64_t id);
        Entity* FindEntity(uint64_t id);
        const std::vector<std::unique_ptr<Entity>>& Entities() const;

    private:
        std::vector<std::unique_ptr<Entity>> mEntities;
    };
}
