// File: MeshEditor/src/scene/Entity.h
#pragma once

#include <string>
#include "engine/Core.h"
#include "scene/Components.h"

namespace MeshEditor
{
class Entity
{
public:
    Entity(int id, const std::string& name);

    int GetId() const { return m_id; }
    const std::string& GetName() const { return m_name; }

    Transform& GetTransform() { return m_transform; }
    const Transform& GetTransform() const { return m_transform; }
    void SetTransform(const Transform& transform) { m_transform = transform; }

    MeshComponent& GetMesh() { return m_mesh; }
    const MeshComponent& GetMesh() const { return m_mesh; }

    MaterialComponent& GetMaterial() { return m_material; }
    const MaterialComponent& GetMaterial() const { return m_material; }

    AABB GetBounds() const;

private:
    int m_id;
    std::string m_name;
    Transform m_transform;
    MeshComponent m_mesh;
    MaterialComponent m_material;
};
}
