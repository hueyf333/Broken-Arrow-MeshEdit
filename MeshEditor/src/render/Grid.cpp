// File: MeshEditor/src/render/Grid.cpp
#include "render/Grid.h"

namespace MeshEditor
{
    bool Grid::Initialize(ID3D11Device* device)
    {
        std::vector<GridVertex> vertices;
        const int gridSize = 20;
        const float step = 1.0f;
        for (int i = -gridSize; i <= gridSize; ++i)
        {
            float x = i * step;
            glm::vec4 color = (i == 0) ? glm::vec4(0.8f, 0.1f, 0.1f, 1.0f) : glm::vec4(0.3f, 0.3f, 0.3f, 1.0f);
            vertices.push_back({glm::vec3(x, 0, -gridSize * step), color});
            vertices.push_back({glm::vec3(x, 0, gridSize * step), color});
        }
        for (int i = -gridSize; i <= gridSize; ++i)
        {
            float z = i * step;
            glm::vec4 color = (i == 0) ? glm::vec4(0.1f, 0.8f, 0.1f, 1.0f) : glm::vec4(0.3f, 0.3f, 0.3f, 1.0f);
            vertices.push_back({glm::vec3(-gridSize * step, 0, z), color});
            vertices.push_back({glm::vec3(gridSize * step, 0, z), color});
        }

        D3D11_BUFFER_DESC desc {};
        desc.ByteWidth = static_cast<UINT>(vertices.size() * sizeof(GridVertex));
        desc.Usage = D3D11_USAGE_DEFAULT;
        desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
        D3D11_SUBRESOURCE_DATA data {};
        data.pSysMem = vertices.data();

        if (FAILED(device->CreateBuffer(&desc, &data, mVertexBuffer.ReleaseAndGetAddressOf())))
        {
            return false;
        }

        mVertexCount = static_cast<uint32_t>(vertices.size());
        return true;
    }

    void Grid::Draw(ID3D11DeviceContext* context) const
    {
        UINT stride = sizeof(GridVertex);
        UINT offset = 0;
        context->IASetVertexBuffers(0, 1, mVertexBuffer.GetAddressOf(), &stride, &offset);
        context->Draw(mVertexCount, 0);
    }
}
