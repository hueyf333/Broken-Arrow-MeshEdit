// File: MeshEditor/src/render/ShaderDX11.cpp
#include "ShaderDX11.h"
#include "engine/Logger.h"
#include <d3dcompiler.h>

namespace MeshEditor
{
bool ShaderDX11::Load(ID3D11Device* device, const std::string& file, const std::string& vsEntry, const std::string& psEntry,
                      const D3D11_INPUT_ELEMENT_DESC* layout, UINT layoutCount)
{
    if (!device)
        return false;

    Microsoft::WRL::ComPtr<ID3DBlob> vsBlob;
    Microsoft::WRL::ComPtr<ID3DBlob> psBlob;
    Microsoft::WRL::ComPtr<ID3DBlob> errors;

    UINT flags = D3DCOMPILE_ENABLE_STRICTNESS;
#ifdef _DEBUG
    flags |= D3DCOMPILE_DEBUG;
#endif

    HRESULT hr = D3DCompileFromFile(std::wstring(file.begin(), file.end()).c_str(), nullptr, nullptr,
                                    vsEntry.c_str(), "vs_5_0", flags, 0, vsBlob.GetAddressOf(), errors.GetAddressOf());
    if (FAILED(hr))
    {
        Logger::Info("Failed to compile vertex shader.");
        return false;
    }

    hr = D3DCompileFromFile(std::wstring(file.begin(), file.end()).c_str(), nullptr, nullptr,
                            psEntry.c_str(), "ps_5_0", flags, 0, psBlob.GetAddressOf(), errors.GetAddressOf());
    if (FAILED(hr))
    {
        Logger::Info("Failed to compile pixel shader.");
        return false;
    }

    device->CreateVertexShader(vsBlob->GetBufferPointer(), vsBlob->GetBufferSize(), nullptr, vertexShader.GetAddressOf());
    device->CreatePixelShader(psBlob->GetBufferPointer(), psBlob->GetBufferSize(), nullptr, pixelShader.GetAddressOf());
    device->CreateInputLayout(layout, layoutCount, vsBlob->GetBufferPointer(), vsBlob->GetBufferSize(), inputLayout.GetAddressOf());
    return true;
}
}
