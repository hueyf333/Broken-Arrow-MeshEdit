#pragma once

#include <DirectXMath.h>
#include <d3d12.h>
#include <dxgi1_6.h>
#include <wrl.h>
#include <memory>
#include <string>
#include <vector>

#include "Mesh.h"

using Microsoft::WRL::ComPtr;

struct Camera {
    DirectX::XMFLOAT3 position{0.0f, 2.5f, -6.0f};
    DirectX::XMFLOAT3 target{0.0f, 0.0f, 0.0f};
    float fov = DirectX::XM_PIDIV4;
    float nearPlane = 0.1f;
    float farPlane = 200.0f;
};

class Renderer {
public:
    explicit Renderer(HWND hwnd);
    ~Renderer();

    void Resize(UINT width, UINT height);
    void Render(const std::vector<std::unique_ptr<MeshObject>> &objects, int selectedIndex, const Camera &camera);

    void SetViewportSize(UINT width, UINT height);
    void SetClearColor(const DirectX::XMFLOAT4 &color);

    void UpdateTexture(MeshObject &object, size_t index);
    void UpdateMeshBuffers(MeshObject &object, size_t index);

    bool IsReady() const;

    ID3D12Device *GetDevice() const { return m_device.Get(); }

private:
    void CreateDeviceResources();
    void CreateSwapChain();
    void CreateRenderTargets();
    void CreateDepthStencil();
    void CreateCommandObjects();
    void CreateFence();
    void CreateDefaultTexture();
    void CreateRootSignature();
    void CreatePipelineState();
    void CreateConstantBuffer();

    void WaitForGpu();
    void MoveToNextFrame();

    struct FrameResource {
        ComPtr<ID3D12CommandAllocator> commandAllocator;
        ComPtr<ID3D12Resource> constantBuffer;
        UINT8 *mappedData = nullptr;
        UINT64 fenceValue = 0;
    };

    struct MeshGpuResources {
        ComPtr<ID3D12Resource> vertexBuffer;
        ComPtr<ID3D12Resource> indexBuffer;
        D3D12_VERTEX_BUFFER_VIEW vbv{};
        D3D12_INDEX_BUFFER_VIEW ibv{};
        UINT indexCount = 0;
    };

    struct TextureResource {
        ComPtr<ID3D12Resource> texture;
        D3D12_CPU_DESCRIPTOR_HANDLE srvHandle{};
        bool valid = false;
    };

    struct ObjectGpuState {
        MeshGpuResources mesh;
        TextureResource texture;
    };

    FrameResource &CurrentFrame();

    HWND m_hwnd = nullptr;
    UINT m_width = 1280;
    UINT m_height = 720;

    static constexpr UINT kFrameCount = 2;
    UINT m_frameIndex = 0;

    ComPtr<IDXGIFactory6> m_factory;
    ComPtr<ID3D12Device> m_device;
    ComPtr<ID3D12CommandQueue> m_commandQueue;
    ComPtr<IDXGISwapChain3> m_swapChain;

    ComPtr<ID3D12DescriptorHeap> m_rtvHeap;
    ComPtr<ID3D12DescriptorHeap> m_dsvHeap;
    ComPtr<ID3D12DescriptorHeap> m_srvHeap;
    UINT m_rtvDescriptorSize = 0;
    UINT m_srvDescriptorSize = 0;

    ComPtr<ID3D12Resource> m_renderTargets[kFrameCount];
    ComPtr<ID3D12Resource> m_depthStencil;

    ComPtr<ID3D12Resource> m_defaultTexture;
    D3D12_CPU_DESCRIPTOR_HANDLE m_defaultSrv{};

    ComPtr<ID3D12GraphicsCommandList> m_commandList;

    FrameResource m_frameResources[kFrameCount];
    ComPtr<ID3D12Fence> m_fence;
    HANDLE m_fenceEvent = nullptr;

    ComPtr<ID3D12RootSignature> m_rootSignature;
    ComPtr<ID3D12PipelineState> m_pipelineState;

    DirectX::XMFLOAT4 m_clearColor{0.1f, 0.1f, 0.15f, 1.0f};

    std::vector<ObjectGpuState> m_objectStates;
};
