// File: MeshEditor/src/assets/AssetDatabase.h
#pragma once

#include <string>
#include <vector>

namespace MeshEditor
{
    class AssetDatabase
    {
    public:
        void AddAsset(const std::string& path);
        const std::vector<std::string>& Assets() const;

    private:
        std::vector<std::string> mAssets;
    };
}
