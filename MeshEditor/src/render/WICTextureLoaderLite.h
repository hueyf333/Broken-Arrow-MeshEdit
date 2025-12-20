// File: MeshEditor/src/render/WICTextureLoaderLite.h
#pragma once

#include <wrl/client.h>
#include <d3d11.h>
#include <string>

namespace MeshEditor
{
class WICTextureLoaderLite
{
public:
    static bool CreateTextureFromFile(ID3D11Device* device, const std::string& file, ID3D11ShaderResourceView** outSrv);
};
}
