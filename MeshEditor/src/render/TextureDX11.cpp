// File: MeshEditor/src/render/TextureDX11.cpp
#include "render/TextureDX11.h"
#include "render/WICTextureLoaderLite.h"

namespace MeshEditor
{
    bool TextureDX11::LoadFromFile(ID3D11Device* device, ID3D11DeviceContext* context, const std::wstring& path)
    {
        return LoadTextureWIC(device, context, path, mView);
    }

    ID3D11ShaderResourceView* TextureDX11::View() const
    {
        return mView.Get();
    }
}
