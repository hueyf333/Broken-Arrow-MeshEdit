// File: MeshEditor/src/render/WICTextureLoaderLite.h
#pragma once

#include <string>
#include <d3d11.h>
#include <wrl/client.h>

namespace mesh
{
bool LoadTextureFromFile(ID3D11Device* device, const std::wstring& filename, Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>& outSrv);
}
