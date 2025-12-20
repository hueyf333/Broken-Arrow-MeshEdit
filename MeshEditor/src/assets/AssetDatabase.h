// File: MeshEditor/src/assets/AssetDatabase.h
#pragma once

#include <string>
#include <vector>

namespace mesh
{
class AssetDatabase
{
public:
    void Add(const std::string& path);
    const std::vector<std::string>& Assets() const;

private:
    std::vector<std::string> assets_;
};
}
