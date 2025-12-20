// File: MeshEditor/src/engine/FileSystem.cpp
#include "FileSystem.h"

#include <windows.h>

namespace mesh
{
std::filesystem::path FileSystem::GetExecutableDir()
{
    wchar_t buffer[MAX_PATH] = {};
    GetModuleFileNameW(nullptr, buffer, MAX_PATH);
    std::filesystem::path path(buffer);
    return path.parent_path();
}

std::filesystem::path FileSystem::GetShaderPath(const std::string& filename)
{
    auto base = GetExecutableDir();
    return base / "shaders" / filename;
}
}
