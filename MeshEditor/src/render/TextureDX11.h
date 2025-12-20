// File: MeshEditor/src/render/TextureDX11.h
#pragma once

#include <d3d11.h>
#include <wrl/client.h>
#include <string>

namespace MeshEditor
{
    class TextureDX11
    {
    public:
        bool LoadFromFile(ID3D11Device* device, ID3D11DeviceContext* context, const std::wstring& path);
        ID3D11ShaderResourceView* View() const;

    private:
        Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> mView;
    };
}
