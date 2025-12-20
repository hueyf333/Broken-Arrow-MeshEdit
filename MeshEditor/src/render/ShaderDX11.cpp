// File: MeshEditor/src/render/ShaderDX11.cpp
#include "render/ShaderDX11.h"

#include <d3dcompiler.h>
#include <vector>

namespace MeshEditor
{
    static bool CompileShader(const std::wstring& path, const std::string& entry, const std::string& target, Microsoft::WRL::ComPtr<ID3DBlob>& blob)
    {
        UINT flags = D3DCOMPILE_ENABLE_STRICTNESS;
#if defined(_DEBUG)
        flags |= D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#endif
        Microsoft::WRL::ComPtr<ID3DBlob> errorBlob;
        HRESULT hr = D3DCompileFromFile(path.c_str(), nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE, entry.c_str(), target.c_str(), flags, 0, blob.ReleaseAndGetAddressOf(), errorBlob.ReleaseAndGetAddressOf());
        return SUCCEEDED(hr);
    }

    bool ShaderDX11::Initialize(ID3D11Device* device, const std::wstring& path, const std::string& vsEntry, const std::string& psEntry, const D3D11_INPUT_ELEMENT_DESC* layout, UINT layoutCount)
    {
        Microsoft::WRL::ComPtr<ID3DBlob> vsBlob;
        Microsoft::WRL::ComPtr<ID3DBlob> psBlob;
        if (!CompileShader(path, vsEntry, "vs_5_0", vsBlob))
        {
            return false;
        }
        if (!CompileShader(path, psEntry, "ps_5_0", psBlob))
        {
            return false;
        }
        if (FAILED(device->CreateVertexShader(vsBlob->GetBufferPointer(), vsBlob->GetBufferSize(), nullptr, mVS.ReleaseAndGetAddressOf())))
        {
            return false;
        }
        if (FAILED(device->CreatePixelShader(psBlob->GetBufferPointer(), psBlob->GetBufferSize(), nullptr, mPS.ReleaseAndGetAddressOf())))
        {
            return false;
        }
        if (layout && layoutCount > 0)
        {
            if (FAILED(device->CreateInputLayout(layout, layoutCount, vsBlob->GetBufferPointer(), vsBlob->GetBufferSize(), mLayout.ReleaseAndGetAddressOf())))
            {
                return false;
            }
        }
        return true;
    }

    void ShaderDX11::Bind(ID3D11DeviceContext* context) const
    {
        context->IASetInputLayout(mLayout.Get());
        context->VSSetShader(mVS.Get(), nullptr, 0);
        context->PSSetShader(mPS.Get(), nullptr, 0);
    }

    ID3D11VertexShader* ShaderDX11::VertexShader() const
    {
        return mVS.Get();
    }

    ID3D11PixelShader* ShaderDX11::PixelShader() const
    {
        return mPS.Get();
    }

    ID3D11InputLayout* ShaderDX11::InputLayout() const
    {
        return mLayout.Get();
    }
}
