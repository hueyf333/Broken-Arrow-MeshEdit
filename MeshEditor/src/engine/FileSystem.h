// File: MeshEditor/src/engine/FileSystem.h
#pragma once

#include <string>

namespace MeshEditor
{
class FileSystem
{
public:
    static std::string GetExecutableDir();
    static std::string GetShaderPath(const std::string& fileName);
};
}
