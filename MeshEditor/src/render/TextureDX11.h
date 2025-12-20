// File: MeshEditor/src/render/TextureDX11.h
#pragma once

#include <string>
#include <d3d11.h>
#include <wrl/client.h>

namespace mesh
{
class TextureDX11
{
public:
    bool Load(ID3D11Device* device, const std::wstring& path);
    ID3D11ShaderResourceView* SRV() const { return srv_.Get(); }

private:
    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> srv_;
};
}
