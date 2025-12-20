// File: MeshEditor/src/engine/FileSystem.h
#pragma once

#include <filesystem>
#include <string>

namespace MeshEditor
{
    class FileSystem
    {
    public:
        static std::filesystem::path ExecutablePath();
        static std::filesystem::path ExecutableDir();
        static std::filesystem::path DataDir();
        static std::filesystem::path ShaderPath(const std::string& filename);
    };
}
