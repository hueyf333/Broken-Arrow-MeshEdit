// File: MeshEditor/src/engine/FileSystem.h
#pragma once

#include <filesystem>
#include <string>

namespace mesh
{
class FileSystem
{
public:
    static std::filesystem::path GetExecutableDir();
    static std::filesystem::path GetShaderPath(const std::string& filename);
};
}
