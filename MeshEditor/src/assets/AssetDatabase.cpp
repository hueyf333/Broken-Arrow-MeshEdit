// File: MeshEditor/src/assets/AssetDatabase.cpp
#include "AssetDatabase.h"

namespace mesh
{
void AssetDatabase::Add(const std::string& path)
{
    assets_.push_back(path);
}

const std::vector<std::string>& AssetDatabase::Assets() const
{
    return assets_;
}
}
