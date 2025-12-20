// File: MeshEditor/src/render/RenderMesh.h
#pragma once

#include <vector>
#include <memory>
#include <glm/glm.hpp>
#include <d3d11.h>
#include <wrl/client.h>

namespace mesh
{
struct Vertex
{
    glm::vec3 position{};
    glm::vec3 normal{};
    glm::vec2 uv{};
};

struct MeshData
{
    std::vector<Vertex> vertices;
    std::vector<uint32_t> indices;
    glm::vec3 boundsMin{0.0f};
    glm::vec3 boundsMax{0.0f};
};

class RenderMesh
{
public:
    bool Initialize(ID3D11Device* device, const MeshData& data);
    void Draw(ID3D11DeviceContext* context) const;

    const glm::vec3& BoundsMin() const { return data_.boundsMin; }
    const glm::vec3& BoundsMax() const { return data_.boundsMax; }
    const MeshData& Data() const { return data_; }

private:
    MeshData data_;
    Microsoft::WRL::ComPtr<ID3D11Buffer> vertexBuffer_;
    Microsoft::WRL::ComPtr<ID3D11Buffer> indexBuffer_;
};
}
