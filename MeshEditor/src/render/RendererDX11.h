// File: MeshEditor/src/render/RendererDX11.h
#pragma once

#include <wrl/client.h>
#include <d3d11.h>
#include <dxgi.h>
#include <QKeyEvent>

namespace MeshEditor
{
class Scene;
class ToolManager;

class RendererDX11
{
public:
    RendererDX11();

    bool Initialize(HWND hwnd, int width, int height);
    void Resize(int width, int height);
    void Render();
    void SetScene(Scene* scene);
    void SetToolManager(ToolManager* manager);
    void ToggleWireframe();
    void ToggleShaded();
    bool IsInitialized() const { return m_initialized; }

    void OnKeyPress(QKeyEvent* event);

private:
    void CreateDeviceAndSwapChain(HWND hwnd, int width, int height);
    void CreateRenderTargets(int width, int height);

    bool m_initialized;
    bool m_wireframe;
    bool m_shaded;

    Scene* m_scene;
    ToolManager* m_tools;

    Microsoft::WRL::ComPtr<ID3D11Device> m_device;
    Microsoft::WRL::ComPtr<ID3D11DeviceContext> m_context;
    Microsoft::WRL::ComPtr<IDXGISwapChain> m_swapChain;
    Microsoft::WRL::ComPtr<ID3D11RenderTargetView> m_rtv;
    Microsoft::WRL::ComPtr<ID3D11DepthStencilView> m_dsv;
};
}
