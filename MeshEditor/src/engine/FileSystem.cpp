// File: MeshEditor/src/engine/FileSystem.cpp
#include "FileSystem.h"
#include <windows.h>
#include <filesystem>

namespace MeshEditor
{
std::string FileSystem::GetExecutableDir()
{
    char path[MAX_PATH] = {};
    GetModuleFileNameA(nullptr, path, MAX_PATH);
    std::filesystem::path exePath(path);
    return exePath.parent_path().string();
}

std::string FileSystem::GetShaderPath(const std::string& fileName)
{
    std::filesystem::path path = GetExecutableDir();
    path /= fileName;
    return path.string();
}
}
