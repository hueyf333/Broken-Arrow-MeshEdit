// File: MeshEditor/src/scene/Scene.cpp
#include "Scene.h"
#include <algorithm>

namespace MeshEditor
{
Scene::Scene()
    : m_nextId(1)
{
    m_selection.BindScene(this);
}

Entity& Scene::CreateEntity(const std::string& name)
{
    m_entities.emplace_back(m_nextId++, name);
    return m_entities.back();
}

void Scene::RemoveEntity(int id)
{
    m_entities.erase(std::remove_if(m_entities.begin(), m_entities.end(), [id](const Entity& e) {
        return e.GetId() == id;
    }), m_entities.end());
    m_selection.Remove(id);
}

Entity* Scene::FindEntity(int id)
{
    for (auto& entity : m_entities)
    {
        if (entity.GetId() == id)
            return &entity;
    }
    return nullptr;
}

const Entity* Scene::FindEntity(int id) const
{
    for (const auto& entity : m_entities)
    {
        if (entity.GetId() == id)
            return &entity;
    }
    return nullptr;
}
}
