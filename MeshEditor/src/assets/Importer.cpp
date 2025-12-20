// File: MeshEditor/src/assets/Importer.cpp
#include "assets/Importer.h"
#include "assets/Import_OBJ.h"
#include "assets/Import_GLTF.h"

#include <filesystem>

namespace MeshEditor
{
    bool Importer::ImportMesh(const std::string& path, Scene& scene, Entity*& outEntity, std::string& outError)
    {
        std::filesystem::path filePath(path);
        std::string ext = filePath.extension().string();
        for (auto& c : ext)
        {
            c = static_cast<char>(tolower(c));
        }

        if (ext == ".obj")
        {
            return ImportOBJ(path, scene, outEntity, outError);
        }
        if (ext == ".gltf" || ext == ".glb")
        {
            return ImportGLTF(path, scene, outEntity, outError);
        }

        outError = "Unsupported file type";
        return false;
    }
}
