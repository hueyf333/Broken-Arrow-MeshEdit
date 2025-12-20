// File: MeshEditor/src/assets/Importer.cpp
#include "Importer.h"
#include "Import_OBJ.h"
#include "Import_GLTF.h"
#include "engine/UndoStack.h"
#include "engine/Command.h"
#include "scene/Scene.h"

namespace MeshEditor
{
bool Importer::Import(const std::string& filePath, Scene& scene, UndoStack& undo)
{
    std::string extension;
    auto dot = filePath.find_last_of('.');
    if (dot != std::string::npos)
    {
        extension = filePath.substr(dot + 1);
    }

    int entityId = -1;
    bool result = false;
    if (extension == "obj")
    {
        Import_OBJ importer;
        result = importer.Import(filePath, scene, entityId);
    }
    else if (extension == "gltf" || extension == "glb")
    {
        Import_GLTF importer;
        result = importer.Import(filePath, scene, entityId);
    }

    if (result && entityId > 0)
    {
        undo.Execute(std::make_unique<ImportCommand>(entityId, &scene));
    }
    return result;
}
}
