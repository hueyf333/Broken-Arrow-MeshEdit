// File: MeshEditor/src/assets/Import_OBJ.cpp
#include "Import_OBJ.h"

#include <tiny_obj_loader.h>
#include <glm/glm.hpp>

namespace mesh
{
bool ImportOBJ(const std::string& path, MeshData& outData)
{
    tinyobj::attrib_t attrib;
    std::vector<tinyobj::shape_t> shapes;
    std::vector<tinyobj::material_t> materials;
    std::string warn;
    std::string err;

    if (!tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, path.c_str()))
    {
        return false;
    }

    outData.vertices.clear();
    outData.indices.clear();

    for (const auto& shape : shapes)
    {
        for (const auto& index : shape.mesh.indices)
        {
            Vertex vertex{};
            vertex.position = {
                attrib.vertices[3 * index.vertex_index + 0],
                attrib.vertices[3 * index.vertex_index + 1],
                attrib.vertices[3 * index.vertex_index + 2]
            };
            if (index.normal_index >= 0)
            {
                vertex.normal = {
                    attrib.normals[3 * index.normal_index + 0],
                    attrib.normals[3 * index.normal_index + 1],
                    attrib.normals[3 * index.normal_index + 2]
                };
            }
            if (index.texcoord_index >= 0)
            {
                vertex.uv = {
                    attrib.texcoords[2 * index.texcoord_index + 0],
                    attrib.texcoords[2 * index.texcoord_index + 1]
                };
            }
            outData.vertices.push_back(vertex);
            outData.indices.push_back(static_cast<uint32_t>(outData.indices.size()));
        }
    }

    if (!outData.vertices.empty())
    {
        outData.boundsMin = outData.vertices.front().position;
        outData.boundsMax = outData.vertices.front().position;
        for (const auto& v : outData.vertices)
        {
            outData.boundsMin = glm::min(outData.boundsMin, v.position);
            outData.boundsMax = glm::max(outData.boundsMax, v.position);
        }
    }

    return !outData.vertices.empty();
}
}
