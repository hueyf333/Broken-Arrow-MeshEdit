// File: MeshEditor/src/assets/AssetDatabase.cpp
#include "assets/AssetDatabase.h"

namespace MeshEditor
{
    void AssetDatabase::AddAsset(const std::string& path)
    {
        mAssets.push_back(path);
    }

    const std::vector<std::string>& AssetDatabase::Assets() const
    {
        return mAssets;
    }
}
