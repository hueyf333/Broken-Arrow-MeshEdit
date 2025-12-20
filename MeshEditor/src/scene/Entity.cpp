// File: MeshEditor/src/scene/Entity.cpp
#include "scene/Entity.h"

#include <atomic>

namespace MeshEditor
{
    namespace
    {
        std::atomic<uint64_t> sNextId {1};
    }

    Entity::Entity(std::string name)
        : mId(sNextId.fetch_add(1)), mName(std::move(name))
    {
    }

    const std::string& Entity::Name() const
    {
        return mName;
    }

    void Entity::SetName(const std::string& name)
    {
        mName = name;
    }

    Transform& Entity::GetTransform()
    {
        return mTransform;
    }

    const Transform& Entity::GetTransform() const
    {
        return mTransform;
    }

    MeshData& Entity::GetMesh()
    {
        return mMesh;
    }

    const MeshData& Entity::GetMesh() const
    {
        return mMesh;
    }

    Material& Entity::GetMaterial()
    {
        return mMaterial;
    }

    const Material& Entity::GetMaterial() const
    {
        return mMaterial;
    }

    uint64_t Entity::Id() const
    {
        return mId;
    }
}
