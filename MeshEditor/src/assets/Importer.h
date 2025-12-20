// File: MeshEditor/src/assets/Importer.h
#pragma once

#include <string>
#include "render/RenderMesh.h"

namespace mesh
{
class Importer
{
public:
    bool Import(const std::string& path, MeshData& outData);
};
}
