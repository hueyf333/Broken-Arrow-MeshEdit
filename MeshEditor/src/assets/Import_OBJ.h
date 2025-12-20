// File: MeshEditor/src/assets/Import_OBJ.h
#pragma once

#include <string>
#include "render/RenderMesh.h"

namespace mesh
{
bool ImportOBJ(const std::string& path, MeshData& outData);
}
