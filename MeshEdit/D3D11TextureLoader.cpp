#include "D3D11TextureLoader.h"

#include <vector>
#include <wincodec.h>

#pragma comment(lib, "windowscodecs.lib")

Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> D3D11TextureLoader::LoadTextureFromFile(
    ID3D11Device* device,
    ID3D11DeviceContext* context,
    const std::wstring& filePath) {
    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> srv;
    if (!device || filePath.empty()) {
        return srv;
    }

    Microsoft::WRL::ComPtr<IWICImagingFactory> factory;
    if (FAILED(CoCreateInstance(CLSID_WICImagingFactory, nullptr, CLSCTX_INPROC_SERVER,
                                IID_PPV_ARGS(&factory)))) {
        return srv;
    }

    Microsoft::WRL::ComPtr<IWICBitmapDecoder> decoder;
    if (FAILED(factory->CreateDecoderFromFilename(filePath.c_str(), nullptr, GENERIC_READ,
                                                  WICDecodeMetadataCacheOnDemand, &decoder))) {
        return srv;
    }

    Microsoft::WRL::ComPtr<IWICBitmapFrameDecode> frame;
    if (FAILED(decoder->GetFrame(0, &frame))) {
        return srv;
    }

    Microsoft::WRL::ComPtr<IWICFormatConverter> converter;
    if (FAILED(factory->CreateFormatConverter(&converter))) {
        return srv;
    }

    if (FAILED(converter->Initialize(frame.Get(), GUID_WICPixelFormat32bppRGBA,
                                     WICBitmapDitherTypeNone, nullptr, 0.0,
                                     WICBitmapPaletteTypeCustom))) {
        return srv;
    }

    UINT width = 0;
    UINT height = 0;
    converter->GetSize(&width, &height);
    if (width == 0 || height == 0) {
        return srv;
    }

    const UINT stride = width * 4;
    const UINT imageSize = stride * height;
    std::vector<BYTE> pixels(imageSize);
    if (FAILED(converter->CopyPixels(nullptr, stride, imageSize, pixels.data()))) {
        return srv;
    }

    D3D11_TEXTURE2D_DESC textureDesc{};
    textureDesc.Width = width;
    textureDesc.Height = height;
    textureDesc.MipLevels = 1;
    textureDesc.ArraySize = 1;
    textureDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    textureDesc.SampleDesc.Count = 1;
    textureDesc.Usage = D3D11_USAGE_DEFAULT;
    textureDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;

    D3D11_SUBRESOURCE_DATA initData{};
    initData.pSysMem = pixels.data();
    initData.SysMemPitch = stride;

    Microsoft::WRL::ComPtr<ID3D11Texture2D> texture;
    if (FAILED(device->CreateTexture2D(&textureDesc, &initData, &texture))) {
        return srv;
    }

    if (FAILED(device->CreateShaderResourceView(texture.Get(), nullptr, &srv))) {
        return {};
    }
    if (context) {
        context->GenerateMips(srv.Get());
    }
    return srv;
}
