// File: MeshEditor/src/render/Grid.cpp
#include "Grid.h"

#include <vector>
#include <glm/glm.hpp>

namespace mesh
{
struct GridVertex
{
    glm::vec3 position;
    glm::vec3 color;
};

bool Grid::Initialize(ID3D11Device* device)
{
    if (!device)
    {
        return false;
    }

    std::vector<GridVertex> vertices;
    const int gridSize = 20;
    const float spacing = 1.0f;
    for (int i = -gridSize; i <= gridSize; ++i)
    {
        float v = static_cast<float>(i) * spacing;
        vertices.push_back({glm::vec3(v, 0.0f, -gridSize * spacing), glm::vec3(0.4f)});
        vertices.push_back({glm::vec3(v, 0.0f, gridSize * spacing), glm::vec3(0.4f)});
        vertices.push_back({glm::vec3(-gridSize * spacing, 0.0f, v), glm::vec3(0.4f)});
        vertices.push_back({glm::vec3(gridSize * spacing, 0.0f, v), glm::vec3(0.4f)});
    }

    vertexCount_ = static_cast<UINT>(vertices.size());

    D3D11_BUFFER_DESC vbDesc{};
    vbDesc.Usage = D3D11_USAGE_DEFAULT;
    vbDesc.ByteWidth = static_cast<UINT>(sizeof(GridVertex) * vertices.size());
    vbDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;

    D3D11_SUBRESOURCE_DATA data{};
    data.pSysMem = vertices.data();

    return SUCCEEDED(device->CreateBuffer(&vbDesc, &data, vertexBuffer_.GetAddressOf()));
}

void Grid::Draw(ID3D11DeviceContext* context) const
{
    if (!context || !vertexBuffer_)
    {
        return;
    }
    UINT stride = sizeof(GridVertex);
    UINT offset = 0;
    context->IASetVertexBuffers(0, 1, vertexBuffer_.GetAddressOf(), &stride, &offset);
    context->Draw(vertexCount_, 0);
}
}
