// File: MeshEditor/src/render/Grid.h
#pragma once

#include <d3d11.h>
#include <wrl/client.h>

namespace mesh
{
class Grid
{
public:
    bool Initialize(ID3D11Device* device);
    void Draw(ID3D11DeviceContext* context) const;

private:
    Microsoft::WRL::ComPtr<ID3D11Buffer> vertexBuffer_;
    UINT vertexCount_ = 0;
};
}
