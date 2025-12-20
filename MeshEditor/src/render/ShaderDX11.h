// File: MeshEditor/src/render/ShaderDX11.h
#pragma once

#include <string>
#include <vector>
#include <d3d11.h>
#include <wrl/client.h>

namespace mesh
{
class ShaderDX11
{
public:
    bool Load(ID3D11Device* device, const std::wstring& path, const std::string& vsEntry, const std::string& psEntry);
    bool LoadWithLayout(ID3D11Device* device, const std::wstring& path, const std::string& vsEntry, const std::string& psEntry,\n+                        const std::vector<D3D11_INPUT_ELEMENT_DESC>& layout);
    void Bind(ID3D11DeviceContext* context) const;

    ID3D11InputLayout* InputLayout() const { return inputLayout_.Get(); }

private:
    Microsoft::WRL::ComPtr<ID3D11VertexShader> vertexShader_;
    Microsoft::WRL::ComPtr<ID3D11PixelShader> pixelShader_;
    Microsoft::WRL::ComPtr<ID3D11InputLayout> inputLayout_;
};
}
