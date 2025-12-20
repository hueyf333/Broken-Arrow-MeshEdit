#pragma once

#include <d3d11.h>
#include <wrl/client.h>

#include <string>

class D3D11TextureLoader {
public:
    static Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> LoadTextureFromFile(
        ID3D11Device* device,
        ID3D11DeviceContext* context,
        const std::wstring& filePath);
};
