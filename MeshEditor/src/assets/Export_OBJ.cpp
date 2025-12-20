// File: MeshEditor/src/assets/Export_OBJ.cpp
#include "Export_OBJ.h"

#include <fstream>

namespace mesh
{
bool ExportOBJ::Export(const std::string& path, const MeshData& data)
{
    std::ofstream file(path);
    if (!file.is_open())
    {
        return false;
    }

    for (const auto& v : data.vertices)
    {
        file << "v " << v.position.x << " " << v.position.y << " " << v.position.z << "\n";
    }

    for (size_t i = 0; i < data.indices.size(); i += 3)
    {
        file << "f " << data.indices[i] + 1 << " " << data.indices[i + 1] + 1 << " " << data.indices[i + 2] + 1 << "\n";
    }

    return true;
}
}
