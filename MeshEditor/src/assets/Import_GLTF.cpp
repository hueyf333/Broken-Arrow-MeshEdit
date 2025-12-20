// File: MeshEditor/src/assets/Import_GLTF.cpp
#include "assets/Import_GLTF.h"
#include "engine/Logger.h"

#define TINYGLTF_IMPLEMENTATION
#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <tiny_gltf.h>

#include <filesystem>
#include <glm/glm.hpp>
#include <cstring>

namespace MeshEditor
{
    template <typename T>
    static std::vector<T> ReadAccessor(const tinygltf::Model& model, const tinygltf::Accessor& accessor)
    {
        const auto& bufferView = model.bufferViews[accessor.bufferView];
        const auto& buffer = model.buffers[bufferView.buffer];
        const uint8_t* dataPtr = buffer.data.data() + bufferView.byteOffset + accessor.byteOffset;
        size_t totalBytes = accessor.count * sizeof(T);
        std::vector<T> out(accessor.count);
        memcpy(out.data(), dataPtr, totalBytes);
        return out;
    }

    bool ImportGLTF(const std::string& path, Scene& scene, Entity*& outEntity, std::string& outError)
    {
        tinygltf::Model model;
        tinygltf::TinyGLTF loader;
        std::string warn;
        bool result = false;
        std::filesystem::path filePath(path);
        if (filePath.extension() == ".glb")
        {
            result = loader.LoadBinaryFromFile(&model, &warn, &outError, path);
        }
        else
        {
            result = loader.LoadASCIIFromFile(&model, &warn, &outError, path);
        }

        if (!warn.empty())
        {
            Logger::Warn(warn);
        }
        if (!result)
        {
            return false;
        }
        if (model.meshes.empty())
        {
            outError = "No meshes found";
            return false;
        }

        const auto& mesh = model.meshes[0];
        if (mesh.primitives.empty())
        {
            outError = "No primitives found";
            return false;
        }

        const auto& primitive = mesh.primitives[0];
        Entity& entity = scene.CreateEntity(mesh.name.empty() ? filePath.stem().string() : mesh.name);
        MeshData& meshData = entity.GetMesh();

        auto posIt = primitive.attributes.find("POSITION");
        if (posIt != primitive.attributes.end())
        {
            const auto& accessor = model.accessors[posIt->second];
            auto positions = ReadAccessor<glm::vec3>(model, accessor);
            meshData.positions = positions;
        }

        auto normIt = primitive.attributes.find("NORMAL");
        if (normIt != primitive.attributes.end())
        {
            const auto& accessor = model.accessors[normIt->second];
            auto normals = ReadAccessor<glm::vec3>(model, accessor);
            meshData.normals = normals;
        }

        auto uvIt = primitive.attributes.find("TEXCOORD_0");
        if (uvIt != primitive.attributes.end())
        {
            const auto& accessor = model.accessors[uvIt->second];
            auto uvs = ReadAccessor<glm::vec2>(model, accessor);
            for (auto& uv : uvs)
            {
                uv.y = 1.0f - uv.y;
            }
            meshData.uvs = uvs;
        }

        if (primitive.indices >= 0)
        {
            const auto& accessor = model.accessors[primitive.indices];
            const auto& bufferView = model.bufferViews[accessor.bufferView];
            const auto& buffer = model.buffers[bufferView.buffer];
            const uint8_t* dataPtr = buffer.data.data() + bufferView.byteOffset + accessor.byteOffset;
            meshData.indices.resize(accessor.count);
            if (accessor.componentType == TINYGLTF_COMPONENT_TYPE_UNSIGNED_SHORT)
            {
                auto src = reinterpret_cast<const uint16_t*>(dataPtr);
                for (size_t i = 0; i < accessor.count; ++i)
                {
                    meshData.indices[i] = src[i];
                }
            }
            else if (accessor.componentType == TINYGLTF_COMPONENT_TYPE_UNSIGNED_INT)
            {
                auto src = reinterpret_cast<const uint32_t*>(dataPtr);
                for (size_t i = 0; i < accessor.count; ++i)
                {
                    meshData.indices[i] = src[i];
                }
            }
            else
            {
                outError = "Unsupported index format";
                return false;
            }
        }
        else
        {
            meshData.indices.resize(meshData.positions.size());
            for (uint32_t i = 0; i < meshData.indices.size(); ++i)
            {
                meshData.indices[i] = i;
            }
        }

        outEntity = &entity;
        Logger::Info("Imported glTF: " + path);
        return true;
    }
}
