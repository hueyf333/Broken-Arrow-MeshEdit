// File: MeshEditor/src/assets/Import_OBJ.h
#pragma once

#include "scene/Scene.h"
#include <string>

namespace MeshEditor
{
    bool ImportOBJ(const std::string& path, Scene& scene, Entity*& outEntity, std::string& outError);
}
