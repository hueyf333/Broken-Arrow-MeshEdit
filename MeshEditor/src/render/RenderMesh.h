// File: MeshEditor/src/render/RenderMesh.h
#pragma once

#include <wrl/client.h>
#include <d3d11.h>
#include "scene/Components.h"

namespace MeshEditor
{
class RenderMesh
{
public:
    void Create(ID3D11Device* device, const MeshComponent& mesh);

    Microsoft::WRL::ComPtr<ID3D11Buffer> vertexBuffer;
    Microsoft::WRL::ComPtr<ID3D11Buffer> indexBuffer;
    uint32_t indexCount = 0;
};
}
