// File: MeshEditor/src/assets/Importer.cpp
#include "Importer.h"

#include "Import_OBJ.h"
#include "Import_GLTF.h"

namespace mesh
{
bool Importer::Import(const std::string& path, MeshData& outData)
{
    if (path.size() >= 4)
    {
        std::string ext = path.substr(path.find_last_of('.') + 1);
        if (ext == "obj")
        {
            return ImportOBJ(path, outData);
        }
        if (ext == "gltf" || ext == "glb")
        {
            return ImportGLTF(path, outData);
        }
    }
    return false;
}
}
