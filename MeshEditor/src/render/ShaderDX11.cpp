// File: MeshEditor/src/render/ShaderDX11.cpp
#include "ShaderDX11.h"

#include <d3dcompiler.h>

namespace mesh
{
bool ShaderDX11::Load(ID3D11Device* device, const std::wstring& path, const std::string& vsEntry, const std::string& psEntry)
{
    std::vector<D3D11_INPUT_ELEMENT_DESC> layout =
    {
        {"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
        {"NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0},
        {"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 24, D3D11_INPUT_PER_VERTEX_DATA, 0}
    };
    return LoadWithLayout(device, path, vsEntry, psEntry, layout);
}

bool ShaderDX11::LoadWithLayout(ID3D11Device* device, const std::wstring& path, const std::string& vsEntry, const std::string& psEntry,
                                const std::vector<D3D11_INPUT_ELEMENT_DESC>& layout)
{
    if (!device)
    {
        return false;
    }

    UINT flags = D3DCOMPILE_ENABLE_STRICTNESS;
#if defined(_DEBUG)
    flags |= D3DCOMPILE_DEBUG;
#endif

    Microsoft::WRL::ComPtr<ID3DBlob> vsBlob;
    Microsoft::WRL::ComPtr<ID3DBlob> psBlob;
    Microsoft::WRL::ComPtr<ID3DBlob> errorBlob;

    if (FAILED(D3DCompileFromFile(path.c_str(), nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE,
                                  vsEntry.c_str(), "vs_5_0", flags, 0, vsBlob.GetAddressOf(), errorBlob.GetAddressOf())))
    {
        return false;
    }

    if (FAILED(D3DCompileFromFile(path.c_str(), nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE,
                                  psEntry.c_str(), "ps_5_0", flags, 0, psBlob.GetAddressOf(), errorBlob.GetAddressOf())))
    {
        return false;
    }

    if (FAILED(device->CreateVertexShader(vsBlob->GetBufferPointer(), vsBlob->GetBufferSize(), nullptr, vertexShader_.GetAddressOf())))
    {
        return false;
    }

    if (FAILED(device->CreatePixelShader(psBlob->GetBufferPointer(), psBlob->GetBufferSize(), nullptr, pixelShader_.GetAddressOf())))
    {
        return false;
    }

    if (FAILED(device->CreateInputLayout(layout.data(), static_cast<UINT>(layout.size()), vsBlob->GetBufferPointer(), vsBlob->GetBufferSize(), inputLayout_.GetAddressOf())))
    {
        return false;
    }

    return true;
}

void ShaderDX11::Bind(ID3D11DeviceContext* context) const
{
    if (!context)
    {
        return;
    }
    context->IASetInputLayout(inputLayout_.Get());
    context->VSSetShader(vertexShader_.Get(), nullptr, 0);
    context->PSSetShader(pixelShader_.Get(), nullptr, 0);
}
}
