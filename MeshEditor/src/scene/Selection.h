// File: MeshEditor/src/scene/Selection.h
#pragma once

#include <vector>

namespace MeshEditor
{
class Scene;
class Entity;

class Selection
{
public:
    void BindScene(Scene* scene);
    void SetPrimary(int id, Scene& scene);
    void Clear();
    void Remove(int id);
    int Count() const { return static_cast<int>(m_selected.size()); }
    bool HasSelection() const { return !m_selected.empty(); }
    Entity* GetPrimary();
    const Entity* GetPrimary() const;

private:
    Scene* m_scene = nullptr;
    std::vector<int> m_selected;
};
}
