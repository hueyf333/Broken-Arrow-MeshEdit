// File: MeshEditor/src/render/RendererDX11.h
#pragma once

#include "render/Camera.h"
#include "render/Grid.h"
#include "render/RenderMesh.h"
#include "render/ShaderDX11.h"
#include "render/TextureDX11.h"
#include "scene/Scene.h"
#include "scene/Selection.h"

#include <d3d11.h>
#include <wrl/client.h>
#include <unordered_map>

namespace MeshEditor
{
    class RendererDX11
    {
    public:
        bool Initialize(HWND hwnd, uint32_t width, uint32_t height);
        void Resize(uint32_t width, uint32_t height);
        void Render(const Scene& scene, const Selection& selection);

        ID3D11Device* Device() const;
        ID3D11DeviceContext* Context() const;
        Camera& GetCamera();

    private:
        void CreateRenderTarget();
        void CreateDepthStencil();
        void EnsureMesh(const Entity& entity);
        void EnsureTexture(const Material& material);

        Microsoft::WRL::ComPtr<ID3D11Device> mDevice;
        Microsoft::WRL::ComPtr<ID3D11DeviceContext> mContext;
        Microsoft::WRL::ComPtr<IDXGISwapChain> mSwapChain;
        Microsoft::WRL::ComPtr<ID3D11RenderTargetView> mRTV;
        Microsoft::WRL::ComPtr<ID3D11DepthStencilView> mDSV;
        Microsoft::WRL::ComPtr<ID3D11Texture2D> mDepthBuffer;
        Microsoft::WRL::ComPtr<ID3D11SamplerState> mSampler;
        Microsoft::WRL::ComPtr<ID3D11Buffer> mCameraCB;
        Microsoft::WRL::ComPtr<ID3D11Buffer> mObjectCB;
        ShaderDX11 mMeshShader;
        ShaderDX11 mGridShader;
        Grid mGrid;
        Camera mCamera;
        uint32_t mWidth = 0;
        uint32_t mHeight = 0;

        struct MeshEntry
        {
            RenderMesh mesh;
            Material material;
        };

        std::unordered_map<uint64_t, MeshEntry> mMeshes;
        std::unordered_map<std::string, TextureDX11> mTextures;
    };
}
