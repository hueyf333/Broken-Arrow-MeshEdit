// File: MeshEditor/src/assets/Import_GLTF.h
#pragma once

#include <string>
#include "render/RenderMesh.h"

namespace mesh
{
bool ImportGLTF(const std::string& path, MeshData& outData);
}
