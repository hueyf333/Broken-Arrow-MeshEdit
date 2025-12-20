// File: MeshEditor/src/render/RenderMesh.h
#pragma once

#include <d3d11.h>
#include <wrl/client.h>
#include <vector>
#include <glm/glm.hpp>

#include "scene/Components.h"

namespace MeshEditor
{
    struct MeshVertex
    {
        glm::vec3 position;
        glm::vec3 normal;
        glm::vec2 uv;
    };

    class RenderMesh
    {
    public:
        bool Initialize(ID3D11Device* device, const MeshData& mesh);
        void Draw(ID3D11DeviceContext* context) const;
        uint32_t IndexCount() const;

    private:
        Microsoft::WRL::ComPtr<ID3D11Buffer> mVertexBuffer;
        Microsoft::WRL::ComPtr<ID3D11Buffer> mIndexBuffer;
        uint32_t mIndexCount = 0;
    };
}
