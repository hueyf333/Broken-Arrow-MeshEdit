// File: MeshEditor/src/assets/Import_GLTF.cpp
#include "Import_GLTF.h"
#include "scene/Scene.h"
#include "scene/Entity.h"
#include <tiny_gltf.h>

namespace MeshEditor
{
bool Import_GLTF::Import(const std::string& filePath, Scene& scene, int& outEntityId)
{
    tinygltf::Model model;
    tinygltf::TinyGLTF loader;
    std::string err;
    std::string warn;

    bool result = false;
    if (filePath.rfind(".glb") != std::string::npos)
    {
        result = loader.LoadBinaryFromFile(&model, &err, &warn, filePath);
    }
    else
    {
        result = loader.LoadASCIIFromFile(&model, &err, &warn, filePath);
    }

    if (!result || model.meshes.empty())
        return false;

    Entity& entity = scene.CreateEntity("glTF Mesh");
    auto& mesh = entity.GetMesh();

    const auto& primitive = model.meshes[0].primitives[0];
    const auto& accessor = model.accessors[primitive.attributes.at("POSITION")];
    const auto& bufferView = model.bufferViews[accessor.bufferView];
    const auto& buffer = model.buffers[bufferView.buffer];

    const float* posData = reinterpret_cast<const float*>(&buffer.data[bufferView.byteOffset + accessor.byteOffset]);
    size_t vertexCount = accessor.count;
    mesh.vertices.resize(vertexCount);
    for (size_t i = 0; i < vertexCount; ++i)
    {
        mesh.vertices[i].position = glm::vec3(posData[i * 3 + 0], posData[i * 3 + 1], posData[i * 3 + 2]);
    }

    if (primitive.indices >= 0)
    {
        const auto& indexAccessor = model.accessors[primitive.indices];
        const auto& indexView = model.bufferViews[indexAccessor.bufferView];
        const auto& indexBuffer = model.buffers[indexView.buffer];
        const uint16_t* idxData = reinterpret_cast<const uint16_t*>(&indexBuffer.data[indexView.byteOffset + indexAccessor.byteOffset]);
        mesh.indices.assign(idxData, idxData + indexAccessor.count);
    }

    outEntityId = entity.GetId();
    return true;
}
}
