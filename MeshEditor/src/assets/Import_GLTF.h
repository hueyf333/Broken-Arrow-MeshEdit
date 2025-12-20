// File: MeshEditor/src/assets/Import_GLTF.h
#pragma once

#include <string>

namespace MeshEditor
{
class Scene;

class Import_GLTF
{
public:
    bool Import(const std::string& filePath, Scene& scene, int& outEntityId);
};
}
