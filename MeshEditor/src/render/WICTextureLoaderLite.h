// File: MeshEditor/src/render/WICTextureLoaderLite.h
#pragma once

#include <d3d11.h>
#include <wrl/client.h>
#include <string>

namespace MeshEditor
{
    bool LoadTextureWIC(ID3D11Device* device, ID3D11DeviceContext* context, const std::wstring& filename, Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>& outView);
}
