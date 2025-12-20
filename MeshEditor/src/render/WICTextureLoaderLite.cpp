// File: MeshEditor/src/render/WICTextureLoaderLite.cpp
#include "WICTextureLoaderLite.h"
#include <wincodec.h>
#include <vector>
#include <windows.h>

namespace MeshEditor
{
bool WICTextureLoaderLite::CreateTextureFromFile(ID3D11Device* device, const std::string& file, ID3D11ShaderResourceView** outSrv)
{
    if (!device || !outSrv)
        return false;

    Microsoft::WRL::ComPtr<IWICImagingFactory> factory;
    if (FAILED(CoCreateInstance(CLSID_WICImagingFactory, nullptr, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&factory))))
        return false;

    Microsoft::WRL::ComPtr<IWICBitmapDecoder> decoder;
    std::wstring wpath(file.begin(), file.end());
    if (FAILED(factory->CreateDecoderFromFilename(wpath.c_str(), nullptr, GENERIC_READ, WICDecodeMetadataCacheOnDemand, &decoder)))
        return false;

    Microsoft::WRL::ComPtr<IWICBitmapFrameDecode> frame;
    decoder->GetFrame(0, &frame);

    Microsoft::WRL::ComPtr<IWICFormatConverter> converter;
    factory->CreateFormatConverter(&converter);
    converter->Initialize(frame.Get(), GUID_WICPixelFormat32bppRGBA, WICBitmapDitherTypeNone, nullptr, 0.0f, WICBitmapPaletteTypeCustom);

    UINT width = 0, height = 0;
    converter->GetSize(&width, &height);
    std::vector<unsigned char> pixels(width * height * 4);
    converter->CopyPixels(nullptr, width * 4, static_cast<UINT>(pixels.size()), pixels.data());

    D3D11_TEXTURE2D_DESC desc = {};
    desc.Width = width;
    desc.Height = height;
    desc.MipLevels = 1;
    desc.ArraySize = 1;
    desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    desc.SampleDesc.Count = 1;
    desc.Usage = D3D11_USAGE_DEFAULT;
    desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;

    D3D11_SUBRESOURCE_DATA initData = {};
    initData.pSysMem = pixels.data();
    initData.SysMemPitch = width * 4;

    Microsoft::WRL::ComPtr<ID3D11Texture2D> texture;
    if (FAILED(device->CreateTexture2D(&desc, &initData, &texture)))
        return false;

    D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
    srvDesc.Format = desc.Format;
    srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
    srvDesc.Texture2D.MipLevels = 1;

    if (FAILED(device->CreateShaderResourceView(texture.Get(), &srvDesc, outSrv)))
        return false;

    return true;
}
}
