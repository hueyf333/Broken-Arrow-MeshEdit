// File: MeshEditor/src/assets/Export_OBJ.cpp
#include "Export_OBJ.h"
#include "scene/Scene.h"
#include "scene/Entity.h"
#include <fstream>

namespace MeshEditor
{
bool Export_OBJ::ExportSelected(const std::string& filePath, const Scene& scene)
{
    const auto* entity = scene.GetSelection().GetPrimary();
    if (!entity)
        return false;

    std::ofstream file(filePath);
    if (!file.is_open())
        return false;

    const auto& mesh = entity->GetMesh();
    for (const auto& v : mesh.vertices)
    {
        file << "v " << v.position.x << " " << v.position.y << " " << v.position.z << "\n";
    }
    for (size_t i = 0; i + 2 < mesh.indices.size(); i += 3)
    {
        file << "f " << mesh.indices[i] + 1 << " " << mesh.indices[i + 1] + 1 << " " << mesh.indices[i + 2] + 1 << "\n";
    }
    return true;
}
}
