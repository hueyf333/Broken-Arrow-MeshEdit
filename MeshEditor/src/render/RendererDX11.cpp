// File: MeshEditor/src/render/RendererDX11.cpp
#include "render/RendererDX11.h"

#include "engine/FileSystem.h"
#include "engine/Logger.h"
#include <DirectXMath.h>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <cstring>

namespace MeshEditor
{
    struct CameraCBData
    {
        DirectX::XMFLOAT4X4 viewProj;
    };

    struct ObjectCBData
    {
        DirectX::XMFLOAT4X4 world;
        DirectX::XMFLOAT4 baseColor;
        uint32_t hasTexture = 0;
        float pad[3] {};
    };

    static DirectX::XMFLOAT4X4 ToXMFLOAT4X4(const glm::mat4& matrix)
    {
        glm::mat4 transposed = glm::transpose(matrix);
        DirectX::XMFLOAT4X4 out;
        std::memcpy(&out, glm::value_ptr(transposed), sizeof(out));
        return out;
    }

    bool RendererDX11::Initialize(HWND hwnd, uint32_t width, uint32_t height)
    {
        mWidth = width;
        mHeight = height;

        DXGI_SWAP_CHAIN_DESC scd {};
        scd.BufferCount = 2;
        scd.BufferDesc.Width = width;
        scd.BufferDesc.Height = height;
        scd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
        scd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
        scd.OutputWindow = hwnd;
        scd.SampleDesc.Count = 1;
        scd.Windowed = TRUE;
        scd.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;

        UINT deviceFlags = D3D11_CREATE_DEVICE_BGRA_SUPPORT;
#if defined(_DEBUG)
        deviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

        if (FAILED(D3D11CreateDeviceAndSwapChain(nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr, deviceFlags, nullptr, 0, D3D11_SDK_VERSION, &scd, mSwapChain.ReleaseAndGetAddressOf(), mDevice.ReleaseAndGetAddressOf(), nullptr, mContext.ReleaseAndGetAddressOf())))
        {
            Logger::Error("Failed to create D3D11 device");
            return false;
        }

        CreateRenderTarget();
        CreateDepthStencil();

        D3D11_BUFFER_DESC cbd {};
        cbd.ByteWidth = sizeof(CameraCBData);
        cbd.Usage = D3D11_USAGE_DEFAULT;
        cbd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
        mDevice->CreateBuffer(&cbd, nullptr, mCameraCB.ReleaseAndGetAddressOf());

        cbd.ByteWidth = sizeof(ObjectCBData);
        mDevice->CreateBuffer(&cbd, nullptr, mObjectCB.ReleaseAndGetAddressOf());

        D3D11_SAMPLER_DESC samplerDesc {};
        samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
        samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
        samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
        samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
        mDevice->CreateSamplerState(&samplerDesc, mSampler.ReleaseAndGetAddressOf());

        D3D11_INPUT_ELEMENT_DESC meshLayout[] =
        {
            {"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
            {"NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0},
            {"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 24, D3D11_INPUT_PER_VERTEX_DATA, 0},
        };

        D3D11_INPUT_ELEMENT_DESC gridLayout[] =
        {
            {"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
            {"COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0},
        };

        auto meshShaderPath = FileSystem::ShaderPath("BasicMesh.hlsl");
        auto gridShaderPath = FileSystem::ShaderPath("Grid.hlsl");
        if (!mMeshShader.Initialize(mDevice.Get(), meshShaderPath.wstring(), "VSMain", "PSMain", meshLayout, 3))
        {
            Logger::Error("Failed to compile BasicMesh.hlsl");
            return false;
        }
        if (!mGridShader.Initialize(mDevice.Get(), gridShaderPath.wstring(), "VSMain", "PSMain", gridLayout, 2))
        {
            Logger::Error("Failed to compile Grid.hlsl");
            return false;
        }

        if (!mGrid.Initialize(mDevice.Get()))
        {
            Logger::Error("Failed to initialize grid");
        }

        mCamera.SetViewport(static_cast<float>(width), static_cast<float>(height));
        return true;
    }

    void RendererDX11::CreateRenderTarget()
    {
        Microsoft::WRL::ComPtr<ID3D11Texture2D> backBuffer;
        mSwapChain->GetBuffer(0, IID_PPV_ARGS(backBuffer.ReleaseAndGetAddressOf()));
        mDevice->CreateRenderTargetView(backBuffer.Get(), nullptr, mRTV.ReleaseAndGetAddressOf());
    }

    void RendererDX11::CreateDepthStencil()
    {
        D3D11_TEXTURE2D_DESC desc {};
        desc.Width = mWidth;
        desc.Height = mHeight;
        desc.MipLevels = 1;
        desc.ArraySize = 1;
        desc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
        desc.SampleDesc.Count = 1;
        desc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
        mDevice->CreateTexture2D(&desc, nullptr, mDepthBuffer.ReleaseAndGetAddressOf());
        mDevice->CreateDepthStencilView(mDepthBuffer.Get(), nullptr, mDSV.ReleaseAndGetAddressOf());
    }

    void RendererDX11::Resize(uint32_t width, uint32_t height)
    {
        if (!mSwapChain)
        {
            return;
        }

        mWidth = width;
        mHeight = height;
        mContext->OMSetRenderTargets(0, nullptr, nullptr);
        mRTV.Reset();
        mDSV.Reset();
        mDepthBuffer.Reset();

        mSwapChain->ResizeBuffers(0, width, height, DXGI_FORMAT_UNKNOWN, 0);
        CreateRenderTarget();
        CreateDepthStencil();
        mCamera.SetViewport(static_cast<float>(width), static_cast<float>(height));
    }

    void RendererDX11::EnsureMesh(const Entity& entity)
    {
        if (mMeshes.find(entity.Id()) != mMeshes.end())
        {
            return;
        }
        MeshEntry entry;
        if (!entry.mesh.Initialize(mDevice.Get(), entity.GetMesh()))
        {
            return;
        }
        entry.material = entity.GetMaterial();
        mMeshes.emplace(entity.Id(), std::move(entry));
    }

    void RendererDX11::EnsureTexture(const Material& material)
    {
        if (material.baseColorTexturePath.empty())
        {
            return;
        }
        if (mTextures.find(material.baseColorTexturePath) != mTextures.end())
        {
            return;
        }
        TextureDX11 texture;
        std::wstring wpath(material.baseColorTexturePath.begin(), material.baseColorTexturePath.end());
        if (texture.LoadFromFile(mDevice.Get(), mContext.Get(), wpath))
        {
            mTextures.emplace(material.baseColorTexturePath, std::move(texture));
        }
    }

    void RendererDX11::Render(const Scene& scene, const Selection& selection)
    {
        if (!mRTV)
        {
            return;
        }

        float clearColor[4] = {0.08f, 0.08f, 0.1f, 1.0f};
        mContext->OMSetRenderTargets(1, mRTV.GetAddressOf(), mDSV.Get());
        mContext->ClearRenderTargetView(mRTV.Get(), clearColor);
        mContext->ClearDepthStencilView(mDSV.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

        D3D11_VIEWPORT viewport {};
        viewport.Width = static_cast<float>(mWidth);
        viewport.Height = static_cast<float>(mHeight);
        viewport.MinDepth = 0.0f;
        viewport.MaxDepth = 1.0f;
        mContext->RSSetViewports(1, &viewport);

        CameraCBData cameraData {};
        cameraData.viewProj = ToXMFLOAT4X4(mCamera.ViewProjection());
        mContext->UpdateSubresource(mCameraCB.Get(), 0, nullptr, &cameraData, 0, 0);
        mContext->VSSetConstantBuffers(0, 1, mCameraCB.GetAddressOf());

        mContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_LINELIST);
        mGridShader.Bind(mContext.Get());
        mGrid.Draw(mContext.Get());

        mContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
        mMeshShader.Bind(mContext.Get());
        mContext->PSSetSamplers(0, 1, mSampler.GetAddressOf());
        mContext->VSSetConstantBuffers(0, 1, mCameraCB.GetAddressOf());
        mContext->PSSetConstantBuffers(1, 1, mObjectCB.GetAddressOf());
        mContext->VSSetConstantBuffers(1, 1, mObjectCB.GetAddressOf());

        for (const auto& entityPtr : scene.Entities())
        {
            const Entity& entity = *entityPtr;
            if (entity.GetMesh().indices.empty())
            {
                continue;
            }
            EnsureMesh(entity);

            auto& entry = mMeshes[entity.Id()];
            entry.material = entity.GetMaterial();
            EnsureTexture(entry.material);

            glm::mat4 world = glm::mat4(1.0f);
            world = glm::translate(world, entity.GetTransform().position);
            world = glm::rotate(world, entity.GetTransform().rotation.x, glm::vec3(1, 0, 0));
            world = glm::rotate(world, entity.GetTransform().rotation.y, glm::vec3(0, 1, 0));
            world = glm::rotate(world, entity.GetTransform().rotation.z, glm::vec3(0, 0, 1));
            world = glm::scale(world, entity.GetTransform().scale);

            ObjectCBData objData {};
            objData.world = ToXMFLOAT4X4(world);
            objData.baseColor = DirectX::XMFLOAT4(entry.material.baseColor.r, entry.material.baseColor.g, entry.material.baseColor.b, entry.material.baseColor.a);
            objData.hasTexture = entry.material.baseColorTexturePath.empty() ? 0 : 1;
            mContext->UpdateSubresource(mObjectCB.Get(), 0, nullptr, &objData, 0, 0);

            ID3D11ShaderResourceView* srv = nullptr;
            if (!entry.material.baseColorTexturePath.empty())
            {
                auto it = mTextures.find(entry.material.baseColorTexturePath);
                if (it != mTextures.end())
                {
                    srv = it->second.View();
                }
            }
            mContext->PSSetShaderResources(0, 1, &srv);

            entry.mesh.Draw(mContext.Get());
        }

        mSwapChain->Present(1, 0);
    }

    ID3D11Device* RendererDX11::Device() const
    {
        return mDevice.Get();
    }

    ID3D11DeviceContext* RendererDX11::Context() const
    {
        return mContext.Get();
    }

    Camera& RendererDX11::GetCamera()
    {
        return mCamera;
    }
}
