// File: MeshEditor/src/assets/Export_OBJ.cpp
#include "assets/Export_OBJ.h"

#include <fstream>

namespace MeshEditor
{
    bool ExportOBJ(const std::string& path, const Entity& entity, std::string& outError)
    {
        std::ofstream file(path);
        if (!file.is_open())
        {
            outError = "Failed to open file for writing";
            return false;
        }

        const auto& mesh = entity.GetMesh();
        for (const auto& v : mesh.positions)
        {
            file << "v " << v.x << " " << v.y << " " << v.z << "\n";
        }

        for (size_t i = 0; i + 2 < mesh.indices.size(); i += 3)
        {
            uint32_t i0 = mesh.indices[i] + 1;
            uint32_t i1 = mesh.indices[i + 1] + 1;
            uint32_t i2 = mesh.indices[i + 2] + 1;
            file << "f " << i0 << " " << i1 << " " << i2 << "\n";
        }

        file.close();
        return true;
    }
}
