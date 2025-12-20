// File: MeshEditor/src/assets/Import_OBJ.cpp
#include "Import_OBJ.h"
#include "scene/Scene.h"
#include "scene/Entity.h"
#include <tiny_obj_loader.h>

namespace MeshEditor
{
bool Import_OBJ::Import(const std::string& filePath, Scene& scene, int& outEntityId)
{
    tinyobj::attrib_t attrib;
    std::vector<tinyobj::shape_t> shapes;
    std::vector<tinyobj::material_t> materials;
    std::string warn, err;

    bool ret = tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, filePath.c_str());
    if (!ret)
        return false;

    Entity& entity = scene.CreateEntity("OBJ Mesh");
    auto& mesh = entity.GetMesh();

    for (const auto& shape : shapes)
    {
        for (const auto& index : shape.mesh.indices)
        {
            Vertex v = {};
            v.position = {
                attrib.vertices[3 * index.vertex_index + 0],
                attrib.vertices[3 * index.vertex_index + 1],
                attrib.vertices[3 * index.vertex_index + 2]
            };
            if (index.normal_index >= 0)
            {
                v.normal = {
                    attrib.normals[3 * index.normal_index + 0],
                    attrib.normals[3 * index.normal_index + 1],
                    attrib.normals[3 * index.normal_index + 2]
                };
            }
            if (index.texcoord_index >= 0)
            {
                v.uv = {
                    attrib.texcoords[2 * index.texcoord_index + 0],
                    attrib.texcoords[2 * index.texcoord_index + 1]
                };
            }
            mesh.indices.push_back(static_cast<uint32_t>(mesh.vertices.size()));
            mesh.vertices.push_back(v);
        }
    }

    outEntityId = entity.GetId();
    return true;
}
}
