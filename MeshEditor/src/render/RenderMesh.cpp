// File: MeshEditor/src/render/RenderMesh.cpp
#include "render/RenderMesh.h"

#include <cstring>

namespace MeshEditor
{
    bool RenderMesh::Initialize(ID3D11Device* device, const MeshData& mesh)
    {
        if (!device || mesh.positions.empty() || mesh.indices.empty())
        {
            return false;
        }

        std::vector<MeshVertex> vertices;
        vertices.reserve(mesh.positions.size());
        for (size_t i = 0; i < mesh.positions.size(); ++i)
        {
            MeshVertex v {};
            v.position = mesh.positions[i];
            v.normal = i < mesh.normals.size() ? mesh.normals[i] : glm::vec3(0, 1, 0);
            v.uv = i < mesh.uvs.size() ? mesh.uvs[i] : glm::vec2(0, 0);
            vertices.push_back(v);
        }

        D3D11_BUFFER_DESC vdesc {};
        vdesc.ByteWidth = static_cast<UINT>(vertices.size() * sizeof(MeshVertex));
        vdesc.Usage = D3D11_USAGE_DEFAULT;
        vdesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
        D3D11_SUBRESOURCE_DATA vdata {};
        vdata.pSysMem = vertices.data();
        if (FAILED(device->CreateBuffer(&vdesc, &vdata, mVertexBuffer.ReleaseAndGetAddressOf())))
        {
            return false;
        }

        D3D11_BUFFER_DESC idesc {};
        idesc.ByteWidth = static_cast<UINT>(mesh.indices.size() * sizeof(uint32_t));
        idesc.Usage = D3D11_USAGE_DEFAULT;
        idesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
        D3D11_SUBRESOURCE_DATA idata {};
        idata.pSysMem = mesh.indices.data();
        if (FAILED(device->CreateBuffer(&idesc, &idata, mIndexBuffer.ReleaseAndGetAddressOf())))
        {
            return false;
        }

        mIndexCount = static_cast<uint32_t>(mesh.indices.size());
        return true;
    }

    void RenderMesh::Draw(ID3D11DeviceContext* context) const
    {
        UINT stride = sizeof(MeshVertex);
        UINT offset = 0;
        context->IASetVertexBuffers(0, 1, mVertexBuffer.GetAddressOf(), &stride, &offset);
        context->IASetIndexBuffer(mIndexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);
        context->DrawIndexed(mIndexCount, 0, 0);
    }

    uint32_t RenderMesh::IndexCount() const
    {
        return mIndexCount;
    }
}
