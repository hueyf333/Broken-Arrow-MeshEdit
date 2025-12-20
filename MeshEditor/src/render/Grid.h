// File: MeshEditor/src/render/Grid.h
#pragma once

#include <d3d11.h>
#include <wrl/client.h>
#include <vector>
#include <glm/glm.hpp>

namespace MeshEditor
{
    struct GridVertex
    {
        glm::vec3 position;
        glm::vec4 color;
    };

    class Grid
    {
    public:
        bool Initialize(ID3D11Device* device);
        void Draw(ID3D11DeviceContext* context) const;

    private:
        Microsoft::WRL::ComPtr<ID3D11Buffer> mVertexBuffer;
        uint32_t mVertexCount = 0;
    };
}
