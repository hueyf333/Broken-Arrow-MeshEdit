// File: MeshEditor/src/scene/Selection.cpp
#include "Selection.h"
#include "Scene.h"
#include <algorithm>

namespace MeshEditor
{
void Selection::BindScene(Scene* scene)
{
    m_scene = scene;
}

void Selection::SetPrimary(int id, Scene& scene)
{
    m_selected.clear();
    if (scene.FindEntity(id))
    {
        m_selected.push_back(id);
    }
}

void Selection::Clear()
{
    m_selected.clear();
}

void Selection::Remove(int id)
{
    m_selected.erase(std::remove(m_selected.begin(), m_selected.end(), id), m_selected.end());
}

Entity* Selection::GetPrimary()
{
    if (!m_scene || m_selected.empty())
        return nullptr;

    return m_scene->FindEntity(m_selected.front());
}

const Entity* Selection::GetPrimary() const
{
    if (!m_scene || m_selected.empty())
        return nullptr;

    return m_scene->FindEntity(m_selected.front());
}
}
