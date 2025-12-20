// File: MeshEditor/src/render/ShaderDX11.h
#pragma once

#include <wrl/client.h>
#include <d3d11.h>
#include <string>

namespace MeshEditor
{
class ShaderDX11
{
public:
    bool Load(ID3D11Device* device, const std::string& file, const std::string& vsEntry, const std::string& psEntry,
              const D3D11_INPUT_ELEMENT_DESC* layout, UINT layoutCount);

    Microsoft::WRL::ComPtr<ID3D11VertexShader> vertexShader;
    Microsoft::WRL::ComPtr<ID3D11PixelShader> pixelShader;
    Microsoft::WRL::ComPtr<ID3D11InputLayout> inputLayout;
};
}
