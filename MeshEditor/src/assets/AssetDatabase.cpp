// File: MeshEditor/src/assets/AssetDatabase.cpp
#include "AssetDatabase.h"

namespace MeshEditor
{
void AssetDatabase::AddAsset(const std::string& name)
{
    m_assets.push_back(name);
}

const std::vector<std::string>& AssetDatabase::GetAssets() const
{
    return m_assets;
}
}
