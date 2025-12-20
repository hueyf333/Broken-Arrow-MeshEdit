// File: MeshEditor/src/assets/Import_OBJ.h
#pragma once

#include <string>

namespace MeshEditor
{
class Scene;

class Import_OBJ
{
public:
    bool Import(const std::string& filePath, Scene& scene, int& outEntityId);
};
}
