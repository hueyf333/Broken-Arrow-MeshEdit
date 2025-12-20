// File: MeshEditor/src/assets/Import_GLTF.cpp
#include "Import_GLTF.h"

#include <tiny_gltf.h>
#include <glm/glm.hpp>
#include <cstring>

namespace mesh
{
static void CopyAttribute(const tinygltf::Model& model, int accessorIndex, std::vector<float>& out)
{
    const auto& accessor = model.accessors[accessorIndex];
    const auto& view = model.bufferViews[accessor.bufferView];
    const auto& buffer = model.buffers[view.buffer];
    const unsigned char* data = buffer.data.data() + view.byteOffset + accessor.byteOffset;
    size_t count = accessor.count * tinygltf::GetNumComponentsInType(accessor.type);
    out.resize(count);
    memcpy(out.data(), data, count * sizeof(float));
}

bool ImportGLTF(const std::string& path, MeshData& outData)
{
    tinygltf::Model model;
    tinygltf::TinyGLTF loader;
    std::string err;
    std::string warn;

    bool ok = false;
    if (path.size() >= 4 && path.substr(path.size() - 4) == ".glb")
    {
        ok = loader.LoadBinaryFromFile(&model, &err, &warn, path);
    }
    else
    {
        ok = loader.LoadASCIIFromFile(&model, &err, &warn, path);
    }
    if (!ok || model.meshes.empty())
    {
        return false;
    }

    const auto& mesh = model.meshes.front();
    if (mesh.primitives.empty())
    {
        return false;
    }

    const auto& primitive = mesh.primitives.front();
    if (primitive.indices < 0)
    {
        return false;
    }

    std::vector<float> positions;
    std::vector<float> normals;
    std::vector<float> uvs;

    auto posIt = primitive.attributes.find("POSITION");
    if (posIt == primitive.attributes.end())
    {
        return false;
    }
    CopyAttribute(model, posIt->second, positions);

    auto normIt = primitive.attributes.find("NORMAL");
    if (normIt != primitive.attributes.end())
    {
        CopyAttribute(model, normIt->second, normals);
    }
    auto uvIt = primitive.attributes.find("TEXCOORD_0");
    if (uvIt != primitive.attributes.end())
    {
        CopyAttribute(model, uvIt->second, uvs);
    }

    const auto& indexAccessor = model.accessors[primitive.indices];
    const auto& indexView = model.bufferViews[indexAccessor.bufferView];
    const auto& indexBuffer = model.buffers[indexView.buffer];
    const unsigned char* indexData = indexBuffer.data.data() + indexView.byteOffset + indexAccessor.byteOffset;

    outData.indices.clear();
    outData.vertices.clear();

    size_t vertexCount = positions.size() / 3;
    outData.vertices.resize(vertexCount);

    for (size_t i = 0; i < vertexCount; ++i)
    {
        Vertex v{};
        v.position = glm::vec3(positions[i * 3 + 0], positions[i * 3 + 1], positions[i * 3 + 2]);
        if (!normals.empty())
        {
            v.normal = glm::vec3(normals[i * 3 + 0], normals[i * 3 + 1], normals[i * 3 + 2]);
        }
        if (!uvs.empty())
        {
            v.uv = glm::vec2(uvs[i * 2 + 0], uvs[i * 2 + 1]);
        }
        outData.vertices[i] = v;
    }

    for (size_t i = 0; i < indexAccessor.count; ++i)
    {
        uint32_t indexValue = 0;
        switch (indexAccessor.componentType)
        {
        case TINYGLTF_COMPONENT_TYPE_UNSIGNED_SHORT:
            indexValue = reinterpret_cast<const uint16_t*>(indexData)[i];
            break;
        case TINYGLTF_COMPONENT_TYPE_UNSIGNED_INT:
            indexValue = reinterpret_cast<const uint32_t*>(indexData)[i];
            break;
        case TINYGLTF_COMPONENT_TYPE_UNSIGNED_BYTE:
            indexValue = reinterpret_cast<const uint8_t*>(indexData)[i];
            break;
        default:
            break;
        }
        outData.indices.push_back(indexValue);
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
