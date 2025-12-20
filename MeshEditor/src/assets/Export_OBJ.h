// File: MeshEditor/src/assets/Export_OBJ.h
#pragma once

#include <string>

namespace MeshEditor
{
class Scene;

class Export_OBJ
{
public:
    bool ExportSelected(const std::string& filePath, const Scene& scene);
};
}
