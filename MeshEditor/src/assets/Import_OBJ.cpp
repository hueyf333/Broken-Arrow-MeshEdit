// File: MeshEditor/src/assets/Import_OBJ.cpp
#include "assets/Import_OBJ.h"
#include "engine/Logger.h"

#define TINYOBJLOADER_IMPLEMENTATION
#include <tiny_obj_loader.h>
#include <filesystem>
#include <glm/glm.hpp>

namespace MeshEditor
{
    bool ImportOBJ(const std::string& path, Scene& scene, Entity*& outEntity, std::string& outError)
    {
        tinyobj::attrib_t attrib;
        std::vector<tinyobj::shape_t> shapes;
        std::vector<tinyobj::material_t> materials;
        std::string warn;

        if (!tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &outError, path.c_str()))
        {
            return false;
        }

        Entity& entity = scene.CreateEntity(std::filesystem::path(path).stem().string());
        MeshData& mesh = entity.GetMesh();

        for (const auto& shape : shapes)
        {
            for (const auto& index : shape.mesh.indices)
            {
                glm::vec3 pos = {
                    attrib.vertices[3 * index.vertex_index + 0],
                    attrib.vertices[3 * index.vertex_index + 1],
                    attrib.vertices[3 * index.vertex_index + 2]};
                mesh.positions.push_back(pos);

                if (!attrib.normals.empty() && index.normal_index >= 0)
                {
                    glm::vec3 norm = {
                        attrib.normals[3 * index.normal_index + 0],
                        attrib.normals[3 * index.normal_index + 1],
                        attrib.normals[3 * index.normal_index + 2]};
                    mesh.normals.push_back(norm);
                }

                if (!attrib.texcoords.empty() && index.texcoord_index >= 0)
                {
                    glm::vec2 uv = {
                        attrib.texcoords[2 * index.texcoord_index + 0],
                        1.0f - attrib.texcoords[2 * index.texcoord_index + 1]};
                    mesh.uvs.push_back(uv);
                }

                mesh.indices.push_back(static_cast<uint32_t>(mesh.indices.size()));
            }
        }

        if (mesh.positions.empty())
        {
            outError = "OBJ has no geometry";
            return false;
        }

        outEntity = &entity;
        Logger::Info("Imported OBJ: " + path);
        return true;
    }
}
