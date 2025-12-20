// File: MeshEditor/src/render/WICTextureLoaderLite.cpp
#include "render/WICTextureLoaderLite.h"

#include <wincodec.h>
#include <vector>

namespace MeshEditor
{
    static Microsoft::WRL::ComPtr<IWICImagingFactory> GetWicFactory()
    {
        Microsoft::WRL::ComPtr<IWICImagingFactory> factory;
        CoCreateInstance(CLSID_WICImagingFactory, nullptr, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(factory.ReleaseAndGetAddressOf()));
        return factory;
    }

    bool LoadTextureWIC(ID3D11Device* device, ID3D11DeviceContext* context, const std::wstring& filename, Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>& outView)
    {
        if (!device || !context)
        {
            return false;
        }

        auto factory = GetWicFactory();
        if (!factory)
        {
            return false;
        }

        Microsoft::WRL::ComPtr<IWICBitmapDecoder> decoder;
        if (FAILED(factory->CreateDecoderFromFilename(filename.c_str(), nullptr, GENERIC_READ, WICDecodeMetadataCacheOnLoad, decoder.ReleaseAndGetAddressOf())))
        {
            return false;
        }

        Microsoft::WRL::ComPtr<IWICBitmapFrameDecode> frame;
        if (FAILED(decoder->GetFrame(0, frame.ReleaseAndGetAddressOf())))
        {
            return false;
        }

        Microsoft::WRL::ComPtr<IWICFormatConverter> converter;
        if (FAILED(factory->CreateFormatConverter(converter.ReleaseAndGetAddressOf())))
        {
            return false;
        }

        if (FAILED(converter->Initialize(frame.Get(), GUID_WICPixelFormat32bppRGBA, WICBitmapDitherTypeNone, nullptr, 0.0, WICBitmapPaletteTypeCustom)))
        {
            return false;
        }

        UINT width = 0;
        UINT height = 0;
        converter->GetSize(&width, &height);
        if (width == 0 || height == 0)
        {
            return false;
        }

        std::vector<uint8_t> pixels(width * height * 4);
        if (FAILED(converter->CopyPixels(nullptr, width * 4, static_cast<UINT>(pixels.size()), pixels.data())))
        {
            return false;
        }

        D3D11_TEXTURE2D_DESC desc {};
        desc.Width = width;
        desc.Height = height;
        desc.MipLevels = 1;
        desc.ArraySize = 1;
        desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
        desc.SampleDesc.Count = 1;
        desc.Usage = D3D11_USAGE_DEFAULT;
        desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;

        D3D11_SUBRESOURCE_DATA initData {};
        initData.pSysMem = pixels.data();
        initData.SysMemPitch = width * 4;

        Microsoft::WRL::ComPtr<ID3D11Texture2D> texture;
        if (FAILED(device->CreateTexture2D(&desc, &initData, texture.ReleaseAndGetAddressOf())))
        {
            return false;
        }

        D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc {};
        srvDesc.Format = desc.Format;
        srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
        srvDesc.Texture2D.MipLevels = 1;

        if (FAILED(device->CreateShaderResourceView(texture.Get(), &srvDesc, outView.ReleaseAndGetAddressOf())))
        {
            return false;
        }

        return true;
    }
}
