#include "MeshImport.h"

#include <vector>

#include "third_party/tinygltf/tiny_gltf.h"
#include "third_party/tinyobjloader/tiny_obj_loader.h"

MeshImportResult ImportObjMesh(const std::string& path) {
    tinyobj::attrib_t attrib;
    std::vector<tinyobj::shape_t> shapes;
    std::vector<tinyobj::material_t> materials;
    std::string warn;
    std::string err;
    const bool ok = tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, path.c_str());
    MeshImportResult result;
    result.success = ok;
    if (!warn.empty()) {
        result.message += warn + "\n";
    }
    if (!err.empty()) {
        result.message += err;
    }
    if (!ok && result.message.empty()) {
        result.message = "OBJ import failed (stub loader).";
    }
    return result;
}

MeshImportResult ImportGltfMesh(const std::string& path) {
    tinygltf::Model model;
    tinygltf::TinyGLTF loader;
    std::string warn;
    std::string err;
    const bool ok = loader.LoadASCIIFromFile(&model, &err, &warn, path);
    MeshImportResult result;
    result.success = ok;
    if (!warn.empty()) {
        result.message += warn + "\n";
    }
    if (!err.empty()) {
        result.message += err;
    }
    if (!ok && result.message.empty()) {
        result.message = "glTF import failed (stub loader).";
    }
    return result;
}
