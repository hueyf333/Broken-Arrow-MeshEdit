// File: MeshEditor/src/assets/AssetDatabase.h
#pragma once

#include <vector>
#include <string>

namespace MeshEditor
{
class AssetDatabase
{
public:
    void AddAsset(const std::string& name);
    const std::vector<std::string>& GetAssets() const;

private:
    std::vector<std::string> m_assets;
};
}
