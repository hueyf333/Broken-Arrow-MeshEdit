// File: MeshEditor/src/render/TextureDX11.h
#pragma once

#include <wrl/client.h>
#include <d3d11.h>
#include <string>

namespace MeshEditor
{
class TextureDX11
{
public:
    bool LoadFromFile(ID3D11Device* device, const std::string& file);

    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> srv;
};
}
