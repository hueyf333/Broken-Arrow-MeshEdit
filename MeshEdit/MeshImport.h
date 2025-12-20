#pragma once

#include <string>

struct MeshImportResult {
    bool success = false;
    std::string message;
};

MeshImportResult ImportObjMesh(const std::string& path);
MeshImportResult ImportGltfMesh(const std::string& path);
