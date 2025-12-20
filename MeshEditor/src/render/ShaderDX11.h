// File: MeshEditor/src/render/ShaderDX11.h
#pragma once

#include <d3d11.h>
#include <wrl/client.h>
#include <string>

namespace MeshEditor
{
    class ShaderDX11
    {
    public:
        bool Initialize(ID3D11Device* device, const std::wstring& path, const std::string& vsEntry, const std::string& psEntry, const D3D11_INPUT_ELEMENT_DESC* layout, UINT layoutCount);
        void Bind(ID3D11DeviceContext* context) const;

        ID3D11VertexShader* VertexShader() const;
        ID3D11PixelShader* PixelShader() const;
        ID3D11InputLayout* InputLayout() const;

    private:
        Microsoft::WRL::ComPtr<ID3D11VertexShader> mVS;
        Microsoft::WRL::ComPtr<ID3D11PixelShader> mPS;
        Microsoft::WRL::ComPtr<ID3D11InputLayout> mLayout;
    };
}
