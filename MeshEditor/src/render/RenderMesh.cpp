// File: MeshEditor/src/render/RenderMesh.cpp
#include "RenderMesh.h"

namespace MeshEditor
{
void RenderMesh::Create(ID3D11Device* device, const MeshComponent& mesh)
{
    if (!device)
        return;

    D3D11_BUFFER_DESC vbDesc = {};
    vbDesc.ByteWidth = static_cast<UINT>(mesh.vertices.size() * sizeof(Vertex));
    vbDesc.Usage = D3D11_USAGE_DEFAULT;
    vbDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;

    D3D11_SUBRESOURCE_DATA vbData = {};
    vbData.pSysMem = mesh.vertices.data();
    device->CreateBuffer(&vbDesc, &vbData, vertexBuffer.GetAddressOf());

    D3D11_BUFFER_DESC ibDesc = {};
    ibDesc.ByteWidth = static_cast<UINT>(mesh.indices.size() * sizeof(uint32_t));
    ibDesc.Usage = D3D11_USAGE_DEFAULT;
    ibDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;

    D3D11_SUBRESOURCE_DATA ibData = {};
    ibData.pSysMem = mesh.indices.data();
    device->CreateBuffer(&ibDesc, &ibData, indexBuffer.GetAddressOf());

    indexCount = static_cast<uint32_t>(mesh.indices.size());
}
}
