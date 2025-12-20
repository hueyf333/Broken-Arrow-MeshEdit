// File: MeshEditor/src/render/RendererDX11.h
#pragma once

#include <unordered_map>
#include <memory>
#include <string>
#include <d3d11.h>
#include <wrl/client.h>
#include <Windows.h>

#include "Camera.h"
#include "Grid.h"
#include "ShaderDX11.h"
#include "TextureDX11.h"
#include "scene/Scene.h"
#include "scene/Selection.h"

namespace mesh
{
class RendererDX11
{
public:
    bool Initialize(HWND hwnd, int width, int height);
    void Resize(int width, int height);
    void Render(const Scene& scene, const Selection& selection);

    Camera& GetCamera() { return camera_; }
    ID3D11Device* GetDevice() const { return device_.Get(); }
    void SetWireframe(bool enabled) { wireframe_ = enabled; }
    void SetShaded(bool enabled) { shaded_ = enabled; }

private:
    void CreateRenderTargets(int width, int height);
    void CreateStates();
    TextureDX11* GetTexture(const std::string& path);

    HWND hwnd_ = nullptr;
    int width_ = 1;
    int height_ = 1;

    Microsoft::WRL::ComPtr<ID3D11Device> device_;
    Microsoft::WRL::ComPtr<ID3D11DeviceContext> context_;
    Microsoft::WRL::ComPtr<IDXGISwapChain> swapChain_;
    Microsoft::WRL::ComPtr<ID3D11RenderTargetView> rtv_;
    Microsoft::WRL::ComPtr<ID3D11DepthStencilView> dsv_;
    Microsoft::WRL::ComPtr<ID3D11Texture2D> depthBuffer_;

    Microsoft::WRL::ComPtr<ID3D11RasterizerState> rasterSolid_;
    Microsoft::WRL::ComPtr<ID3D11RasterizerState> rasterWire_;
    Microsoft::WRL::ComPtr<ID3D11SamplerState> sampler_;

    Microsoft::WRL::ComPtr<ID3D11Buffer> constantBuffer_;

    ShaderDX11 meshShader_;
    ShaderDX11 gridShader_;
    Grid grid_;
    Camera camera_;

    std::unordered_map<std::string, std::unique_ptr<TextureDX11>> textures_;

    bool wireframe_ = false;
    bool shaded_ = true;
};
}
