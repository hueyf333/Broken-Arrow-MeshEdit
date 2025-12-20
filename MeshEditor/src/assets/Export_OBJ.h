// File: MeshEditor/src/assets/Export_OBJ.h
#pragma once

#include "scene/Entity.h"
#include <string>

namespace MeshEditor
{
    bool ExportOBJ(const std::string& path, const Entity& entity, std::string& outError);
}
