// File: MeshEditor/src/engine/FileSystem.cpp
#include "engine/FileSystem.h"
#include <windows.h>

namespace MeshEditor
{
    std::filesystem::path FileSystem::ExecutablePath()
    {
        wchar_t buffer[MAX_PATH] = {};
        GetModuleFileNameW(nullptr, buffer, MAX_PATH);
        return std::filesystem::path(buffer);
    }

    std::filesystem::path FileSystem::ExecutableDir()
    {
        return ExecutablePath().parent_path();
    }

    std::filesystem::path FileSystem::DataDir()
    {
        return ExecutableDir() / "data";
    }

    std::filesystem::path FileSystem::ShaderPath(const std::string& filename)
    {
        return DataDir() / "shaders" / filename;
    }
}
