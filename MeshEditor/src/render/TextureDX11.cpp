// File: MeshEditor/src/render/TextureDX11.cpp
#include "TextureDX11.h"
#include "WICTextureLoaderLite.h"

namespace MeshEditor
{
bool TextureDX11::LoadFromFile(ID3D11Device* device, const std::string& file)
{
    if (!device)
        return false;

    return WICTextureLoaderLite::CreateTextureFromFile(device, file, srv.GetAddressOf());
}
}
