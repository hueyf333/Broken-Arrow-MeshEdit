// File: MeshEditor/src/scene/Entity.h
#pragma once

#include "scene/Components.h"
#include <string>

namespace MeshEditor
{
    class Entity
    {
    public:
        Entity() = default;
        explicit Entity(std::string name);

        const std::string& Name() const;
        void SetName(const std::string& name);

        Transform& GetTransform();
        const Transform& GetTransform() const;

        MeshData& GetMesh();
        const MeshData& GetMesh() const;

        Material& GetMaterial();
        const Material& GetMaterial() const;

        uint64_t Id() const;

    private:
        uint64_t mId = 0;
        std::string mName;
        Transform mTransform {};
        MeshData mMesh {};
        Material mMaterial {};
    };
}
