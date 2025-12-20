// File: MeshEditor/src/assets/Import_GLTF.h
#pragma once

#include "scene/Scene.h"
#include <string>

namespace MeshEditor
{
    bool ImportGLTF(const std::string& path, Scene& scene, Entity*& outEntity, std::string& outError);
}
