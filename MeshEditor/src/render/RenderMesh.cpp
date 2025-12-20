// File: MeshEditor/src/render/RenderMesh.cpp
#include "RenderMesh.h"

namespace mesh
{
bool RenderMesh::Initialize(ID3D11Device* device, const MeshData& data)
{
    data_ = data;
    if (!device || data.vertices.empty() || data.indices.empty())
    {
        return false;
    }

    D3D11_BUFFER_DESC vbDesc{};
    vbDesc.Usage = D3D11_USAGE_DEFAULT;
    vbDesc.ByteWidth = static_cast<UINT>(sizeof(Vertex) * data.vertices.size());
    vbDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;

    D3D11_SUBRESOURCE_DATA vbData{};
    vbData.pSysMem = data.vertices.data();

    if (FAILED(device->CreateBuffer(&vbDesc, &vbData, vertexBuffer_.GetAddressOf())))
    {
        return false;
    }

    D3D11_BUFFER_DESC ibDesc{};
    ibDesc.Usage = D3D11_USAGE_DEFAULT;
    ibDesc.ByteWidth = static_cast<UINT>(sizeof(uint32_t) * data.indices.size());
    ibDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;

    D3D11_SUBRESOURCE_DATA ibData{};
    ibData.pSysMem = data.indices.data();

    if (FAILED(device->CreateBuffer(&ibDesc, &ibData, indexBuffer_.GetAddressOf())))
    {
        return false;
    }

    return true;
}

void RenderMesh::Draw(ID3D11DeviceContext* context) const
{
    if (!context || !vertexBuffer_ || !indexBuffer_)
    {
        return;
    }

    UINT stride = sizeof(Vertex);
    UINT offset = 0;
    context->IASetVertexBuffers(0, 1, vertexBuffer_.GetAddressOf(), &stride, &offset);
    context->IASetIndexBuffer(indexBuffer_.Get(), DXGI_FORMAT_R32_UINT, 0);
    context->DrawIndexed(static_cast<UINT>(data_.indices.size()), 0, 0);
}
}
