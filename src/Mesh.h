#pragma once

#include <DirectXMath.h>
#include <string>
#include <vector>

struct Vertex {
    DirectX::XMFLOAT3 position{};
    DirectX::XMFLOAT3 normal{};
    DirectX::XMFLOAT2 uv{};
};

struct Material {
    DirectX::XMFLOAT4 baseColor{0.9f, 0.9f, 0.9f, 1.0f};
    std::wstring texturePath;
    bool hasTexture = false;
};

struct Mesh {
    std::vector<Vertex> vertices;
    std::vector<uint32_t> indices;
};

struct Transform {
    DirectX::XMFLOAT3 position{0.0f, 0.0f, 0.0f};
    DirectX::XMFLOAT3 rotation{0.0f, 0.0f, 0.0f};
    DirectX::XMFLOAT3 scale{1.0f, 1.0f, 1.0f};
};

struct MeshObject {
    std::string name;
    Mesh mesh;
    Transform transform;
    Material material;
};

namespace MeshOps {
    void ComputeNormals(Mesh &mesh);
    void Subdivide(Mesh &mesh);
    void ApplyTransform(Mesh &mesh, const Transform &transform);
}
