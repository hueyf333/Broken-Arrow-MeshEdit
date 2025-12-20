// File: MeshEditor/src/render/TextureDX11.cpp
#include "TextureDX11.h"

#include "WICTextureLoaderLite.h"

namespace mesh
{
bool TextureDX11::Load(ID3D11Device* device, const std::wstring& path)
{
    return LoadTextureFromFile(device, path, srv_);
}
}
