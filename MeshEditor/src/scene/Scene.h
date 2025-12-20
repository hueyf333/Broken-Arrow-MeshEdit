// File: MeshEditor/src/scene/Scene.h
#pragma once

#include <vector>
#include <memory>
#include "scene/Entity.h"
#include "scene/Selection.h"

namespace MeshEditor
{
class Scene
{
public:
    Scene();

    Entity& CreateEntity(const std::string& name);
    void RemoveEntity(int id);
    Entity* FindEntity(int id);
    const Entity* FindEntity(int id) const;

    const std::vector<Entity>& GetEntities() const { return m_entities; }
    Selection& GetSelection() { return m_selection; }
    const Selection& GetSelection() const { return m_selection; }

private:
    int m_nextId;
    std::vector<Entity> m_entities;
    Selection m_selection;
};
}
