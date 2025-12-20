// File: MeshEditor/src/render/RendererDX11.cpp
#include "RendererDX11.h"
#include "scene/Scene.h"
#include "tools/ToolManager.h"
#include "engine/Logger.h"

namespace MeshEditor
{
RendererDX11::RendererDX11()
    : m_initialized(false)
    , m_wireframe(false)
    , m_shaded(true)
    , m_scene(nullptr)
    , m_tools(nullptr)
{
}

bool RendererDX11::Initialize(HWND hwnd, int width, int height)
{
    CreateDeviceAndSwapChain(hwnd, width, height);
    CreateRenderTargets(width, height);
    m_initialized = true;
    return true;
}

void RendererDX11::CreateDeviceAndSwapChain(HWND hwnd, int width, int height)
{
    DXGI_SWAP_CHAIN_DESC desc = {};
    desc.BufferCount = 2;
    desc.BufferDesc.Width = width;
    desc.BufferDesc.Height = height;
    desc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    desc.OutputWindow = hwnd;
    desc.SampleDesc.Count = 1;
    desc.Windowed = TRUE;
    desc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

    UINT flags = 0;
#if defined(_DEBUG)
    flags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

    D3D_FEATURE_LEVEL featureLevels[] = { D3D_FEATURE_LEVEL_11_0 };
    D3D_FEATURE_LEVEL createdLevel;
    HRESULT hr = D3D11CreateDeviceAndSwapChain(
        nullptr,
        D3D_DRIVER_TYPE_HARDWARE,
        nullptr,
        flags,
        featureLevels,
        1,
        D3D11_SDK_VERSION,
        &desc,
        m_swapChain.GetAddressOf(),
        m_device.GetAddressOf(),
        &createdLevel,
        m_context.GetAddressOf());

    if (FAILED(hr))
    {
        Logger::Info("Failed to create D3D11 device.");
    }
}

void RendererDX11::CreateRenderTargets(int width, int height)
{
    Microsoft::WRL::ComPtr<ID3D11Texture2D> backBuffer;
    m_swapChain->GetBuffer(0, IID_PPV_ARGS(&backBuffer));
    m_device->CreateRenderTargetView(backBuffer.Get(), nullptr, m_rtv.GetAddressOf());

    D3D11_TEXTURE2D_DESC depthDesc = {};
    depthDesc.Width = width;
    depthDesc.Height = height;
    depthDesc.MipLevels = 1;
    depthDesc.ArraySize = 1;
    depthDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
    depthDesc.SampleDesc.Count = 1;
    depthDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;

    Microsoft::WRL::ComPtr<ID3D11Texture2D> depthBuffer;
    m_device->CreateTexture2D(&depthDesc, nullptr, depthBuffer.GetAddressOf());
    m_device->CreateDepthStencilView(depthBuffer.Get(), nullptr, m_dsv.GetAddressOf());
}

void RendererDX11::Resize(int width, int height)
{
    if (!m_swapChain)
        return;

    m_rtv.Reset();
    m_dsv.Reset();
    m_swapChain->ResizeBuffers(0, width, height, DXGI_FORMAT_UNKNOWN, 0);
    CreateRenderTargets(width, height);
}

void RendererDX11::Render()
{
    if (!m_context)
        return;

    float clearColor[4] = {0.08f, 0.08f, 0.1f, 1.0f};
    m_context->OMSetRenderTargets(1, m_rtv.GetAddressOf(), m_dsv.Get());
    m_context->ClearRenderTargetView(m_rtv.Get(), clearColor);
    m_context->ClearDepthStencilView(m_dsv.Get(), D3D11_CLEAR_DEPTH, 1.0f, 0);

    // TODO: render grid, meshes, gizmos.

    m_swapChain->Present(1, 0);
}

void RendererDX11::SetScene(Scene* scene)
{
    m_scene = scene;
}

void RendererDX11::SetToolManager(ToolManager* manager)
{
    m_tools = manager;
}

void RendererDX11::ToggleWireframe()
{
    m_wireframe = !m_wireframe;
}

void RendererDX11::ToggleShaded()
{
    m_shaded = !m_shaded;
}

void RendererDX11::OnKeyPress(QKeyEvent* event)
{
    if (!event)
        return;

    if (event->key() == Qt::Key_F)
    {
        // TODO: frame selected.
    }
}
}
