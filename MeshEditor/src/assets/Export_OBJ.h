// File: MeshEditor/src/assets/Export_OBJ.h
#pragma once

#include <string>
#include "render/RenderMesh.h"

namespace mesh
{
class ExportOBJ
{
public:
    bool Export(const std::string& path, const MeshData& data);
};
}
