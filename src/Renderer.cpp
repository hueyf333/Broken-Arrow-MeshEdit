#include "Renderer.h"

#include <DirectXMath.h>
#include <d3dcompiler.h>
#include <wincodec.h>
#include <cstring>
#include <stdexcept>

using namespace DirectX;

namespace {
    struct alignas(256) ObjectConstants {
        XMFLOAT4X4 model;
        XMFLOAT4X4 view;
        XMFLOAT4X4 proj;
        XMFLOAT4 color;
        UINT useTexture;
        UINT padding[3];
    };

    inline void ThrowIfFailed(HRESULT hr) {
        if (FAILED(hr)) {
            throw std::runtime_error("D3D12 call failed.");
        }
    }

    D3D12_CPU_DESCRIPTOR_HANDLE OffsetCpuHandle(D3D12_CPU_DESCRIPTOR_HANDLE handle, UINT offset, UINT increment) {
        handle.ptr += static_cast<SIZE_T>(offset) * increment;
        return handle;
    }

    D3D12_GPU_DESCRIPTOR_HANDLE OffsetGpuHandle(D3D12_GPU_DESCRIPTOR_HANDLE handle, UINT offset, UINT increment) {
        handle.ptr += static_cast<UINT64>(offset) * increment;
        return handle;
    }

    D3D12_HEAP_PROPERTIES HeapProps(D3D12_HEAP_TYPE type) {
        D3D12_HEAP_PROPERTIES props = {};
        props.Type = type;
        props.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
        props.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
        props.CreationNodeMask = 1;
        props.VisibleNodeMask = 1;
        return props;
    }

    D3D12_RESOURCE_DESC BufferDesc(UINT64 size) {
        D3D12_RESOURCE_DESC desc = {};
        desc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
        desc.Width = size;
        desc.Height = 1;
        desc.DepthOrArraySize = 1;
        desc.MipLevels = 1;
        desc.SampleDesc.Count = 1;
        desc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
        return desc;
    }

    void UploadTexture(ID3D12Device *device, ID3D12GraphicsCommandList *commandList, ID3D12Resource *dest, ID3D12Resource *upload, const D3D12_SUBRESOURCE_DATA &data) {
        D3D12_RESOURCE_DESC desc = dest->GetDesc();
        D3D12_PLACED_SUBRESOURCE_FOOTPRINT footprint = {};
        UINT numRows = 0;
        UINT64 rowSize = 0;
        UINT64 uploadSize = 0;
        device->GetCopyableFootprints(&desc, 0, 1, 0, &footprint, &numRows, &rowSize, &uploadSize);

        UINT8 *mapped = nullptr;
        D3D12_RANGE range = {0, 0};
        ThrowIfFailed(upload->Map(0, &range, reinterpret_cast<void **>(&mapped)));
        for (UINT row = 0; row < numRows; ++row) {
            memcpy(mapped + footprint.Offset + row * footprint.Footprint.RowPitch, static_cast<const uint8_t *>(data.pData) + row * data.RowPitch, data.RowPitch);
        }
        upload->Unmap(0, nullptr);

        D3D12_TEXTURE_COPY_LOCATION dst = {};
        dst.pResource = dest;
        dst.Type = D3D12_TEXTURE_COPY_TYPE_SUBRESOURCE_INDEX;
        dst.SubresourceIndex = 0;

        D3D12_TEXTURE_COPY_LOCATION src = {};
        src.pResource = upload;
        src.Type = D3D12_TEXTURE_COPY_TYPE_PLACED_FOOTPRINT;
        src.PlacedFootprint = footprint;

        commandList->CopyTextureRegion(&dst, 0, 0, 0, &src, nullptr);
    }

    D3D12_RASTERIZER_DESC DefaultRasterizer() {
        D3D12_RASTERIZER_DESC desc = {};
        desc.FillMode = D3D12_FILL_MODE_SOLID;
        desc.CullMode = D3D12_CULL_MODE_BACK;
        desc.FrontCounterClockwise = FALSE;
        desc.DepthBias = D3D12_DEFAULT_DEPTH_BIAS;
        desc.DepthBiasClamp = D3D12_DEFAULT_DEPTH_BIAS_CLAMP;
        desc.SlopeScaledDepthBias = D3D12_DEFAULT_SLOPE_SCALED_DEPTH_BIAS;
        desc.DepthClipEnable = TRUE;
        desc.MultisampleEnable = FALSE;
        desc.AntialiasedLineEnable = FALSE;
        desc.ForcedSampleCount = 0;
        desc.ConservativeRaster = D3D12_CONSERVATIVE_RASTERIZATION_MODE_OFF;
        return desc;
    }

    D3D12_BLEND_DESC DefaultBlend() {
        D3D12_BLEND_DESC desc = {};
        desc.AlphaToCoverageEnable = FALSE;
        desc.IndependentBlendEnable = FALSE;
        const D3D12_RENDER_TARGET_BLEND_DESC rtBlend = {
            FALSE, FALSE,
            D3D12_BLEND_ONE, D3D12_BLEND_ZERO, D3D12_BLEND_OP_ADD,
            D3D12_BLEND_ONE, D3D12_BLEND_ZERO, D3D12_BLEND_OP_ADD,
            D3D12_LOGIC_OP_NOOP,
            D3D12_COLOR_WRITE_ENABLE_ALL};
        for (auto &rt : desc.RenderTarget) {
            rt = rtBlend;
        }
        return desc;
    }

    D3D12_DEPTH_STENCIL_DESC DefaultDepthStencil() {
        D3D12_DEPTH_STENCIL_DESC desc = {};
        desc.DepthEnable = TRUE;
        desc.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;
        desc.DepthFunc = D3D12_COMPARISON_FUNC_LESS;
        desc.StencilEnable = FALSE;
        return desc;
    }

    ComPtr<ID3DBlob> CompileShader(const std::wstring &path, const std::string &entry, const std::string &target) {
        UINT flags = D3DCOMPILE_ENABLE_STRICTNESS;
#ifdef _DEBUG
        flags |= D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#endif
        ComPtr<ID3DBlob> shader;
        ComPtr<ID3DBlob> errors;
        HRESULT hr = D3DCompileFromFile(path.c_str(), nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE, entry.c_str(), target.c_str(), flags, 0, &shader, &errors);
        if (FAILED(hr)) {
            if (errors) {
                OutputDebugStringA(static_cast<const char *>(errors->GetBufferPointer()));
            }
            ThrowIfFailed(hr);
        }
        return shader;
    }

    DXGI_FORMAT GetSRGBFormat(DXGI_FORMAT format) {
        switch (format) {
        case DXGI_FORMAT_R8G8B8A8_UNORM:
            return DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
        default:
            return format;
        }
    }
}

Renderer::Renderer(HWND hwnd) : m_hwnd(hwnd) {
    CreateDeviceResources();
    CreateCommandObjects();
    CreateDefaultTexture();
    CreateSwapChain();
    CreateRenderTargets();
    CreateDepthStencil();
    CreateFence();
    CreateRootSignature();
    CreatePipelineState();
    CreateConstantBuffer();
}

Renderer::~Renderer() {
    WaitForGpu();
    if (m_fenceEvent) {
        CloseHandle(m_fenceEvent);
    }
}

bool Renderer::IsReady() const {
    return m_device != nullptr;
}

void Renderer::CreateDeviceResources() {
    UINT dxgiFactoryFlags = 0;
#ifdef _DEBUG
    ComPtr<ID3D12Debug> debug;
    if (SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(&debug)))) {
        debug->EnableDebugLayer();
        dxgiFactoryFlags |= DXGI_CREATE_FACTORY_DEBUG;
    }
#endif

    ThrowIfFailed(CreateDXGIFactory2(dxgiFactoryFlags, IID_PPV_ARGS(&m_factory)));

    ComPtr<IDXGIAdapter1> adapter;
    for (UINT i = 0; DXGI_ERROR_NOT_FOUND != m_factory->EnumAdapters1(i, &adapter); ++i) {
        DXGI_ADAPTER_DESC1 desc;
        adapter->GetDesc1(&desc);
        if (desc.Flags & DXGI_ADAPTER_FLAG_SOFTWARE) {
            continue;
        }
        if (SUCCEEDED(D3D12CreateDevice(adapter.Get(), D3D_FEATURE_LEVEL_11_0, IID_PPV_ARGS(&m_device)))) {
            break;
        }
    }

    if (!m_device) {
        ComPtr<IDXGIAdapter> warp;
        ThrowIfFailed(m_factory->EnumWarpAdapter(IID_PPV_ARGS(&warp)));
        ThrowIfFailed(D3D12CreateDevice(warp.Get(), D3D_FEATURE_LEVEL_11_0, IID_PPV_ARGS(&m_device)));
    }

    D3D12_DESCRIPTOR_HEAP_DESC rtvDesc = {};
    rtvDesc.NumDescriptors = kFrameCount;
    rtvDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
    ThrowIfFailed(m_device->CreateDescriptorHeap(&rtvDesc, IID_PPV_ARGS(&m_rtvHeap)));
    m_rtvDescriptorSize = m_device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);

    D3D12_DESCRIPTOR_HEAP_DESC dsvDesc = {};
    dsvDesc.NumDescriptors = 1;
    dsvDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
    ThrowIfFailed(m_device->CreateDescriptorHeap(&dsvDesc, IID_PPV_ARGS(&m_dsvHeap)));

    D3D12_DESCRIPTOR_HEAP_DESC srvDesc = {};
    srvDesc.NumDescriptors = 128;
    srvDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
    srvDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
    ThrowIfFailed(m_device->CreateDescriptorHeap(&srvDesc, IID_PPV_ARGS(&m_srvHeap)));
    m_srvDescriptorSize = m_device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
}

void Renderer::CreateCommandObjects() {
    D3D12_COMMAND_QUEUE_DESC queueDesc = {};
    queueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
    ThrowIfFailed(m_device->CreateCommandQueue(&queueDesc, IID_PPV_ARGS(&m_commandQueue)));

    for (auto &frame : m_frameResources) {
        ThrowIfFailed(m_device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&frame.commandAllocator)));
    }

    ThrowIfFailed(m_device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, m_frameResources[0].commandAllocator.Get(), nullptr, IID_PPV_ARGS(&m_commandList)));
    ThrowIfFailed(m_commandList->Close());
}

void Renderer::CreateSwapChain() {
    RECT rect;
    GetClientRect(m_hwnd, &rect);
    m_width = rect.right - rect.left;
    m_height = rect.bottom - rect.top;

    DXGI_SWAP_CHAIN_DESC1 desc = {};
    desc.BufferCount = kFrameCount;
    desc.Width = m_width;
    desc.Height = m_height;
    desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    desc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
    desc.SampleDesc.Count = 1;

    ComPtr<IDXGISwapChain1> swapChain;
    ThrowIfFailed(m_factory->CreateSwapChainForHwnd(m_commandQueue.Get(), m_hwnd, &desc, nullptr, nullptr, &swapChain));
    ThrowIfFailed(swapChain.As(&m_swapChain));
    m_frameIndex = m_swapChain->GetCurrentBackBufferIndex();
}

void Renderer::CreateRenderTargets() {
    D3D12_CPU_DESCRIPTOR_HANDLE handle = m_rtvHeap->GetCPUDescriptorHandleForHeapStart();
    for (UINT i = 0; i < kFrameCount; ++i) {
        ThrowIfFailed(m_swapChain->GetBuffer(i, IID_PPV_ARGS(&m_renderTargets[i])));
        m_device->CreateRenderTargetView(m_renderTargets[i].Get(), nullptr, handle);
        handle = OffsetCpuHandle(handle, 1, m_rtvDescriptorSize);
    }
}

void Renderer::CreateDepthStencil() {
    D3D12_RESOURCE_DESC desc = {};
    desc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
    desc.Width = m_width;
    desc.Height = m_height;
    desc.MipLevels = 1;
    desc.DepthOrArraySize = 1;
    desc.Format = DXGI_FORMAT_D32_FLOAT;
    desc.SampleDesc.Count = 1;
    desc.Flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;

    D3D12_CLEAR_VALUE clear = {};
    clear.Format = DXGI_FORMAT_D32_FLOAT;
    clear.DepthStencil.Depth = 1.0f;

    D3D12_HEAP_PROPERTIES heapProps = HeapProps(D3D12_HEAP_TYPE_DEFAULT);
    ThrowIfFailed(m_device->CreateCommittedResource(
        &heapProps,
        D3D12_HEAP_FLAG_NONE,
        &desc,
        D3D12_RESOURCE_STATE_DEPTH_WRITE,
        &clear,
        IID_PPV_ARGS(&m_depthStencil)));

    m_device->CreateDepthStencilView(m_depthStencil.Get(), nullptr, m_dsvHeap->GetCPUDescriptorHandleForHeapStart());
}

void Renderer::CreateFence() {
    ThrowIfFailed(m_device->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&m_fence)));
    m_fenceEvent = CreateEvent(nullptr, FALSE, FALSE, nullptr);
}

void Renderer::CreateDefaultTexture() {
    uint32_t white = 0xFFFFFFFF;
    D3D12_RESOURCE_DESC texDesc = {};
    texDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
    texDesc.Width = 1;
    texDesc.Height = 1;
    texDesc.DepthOrArraySize = 1;
    texDesc.MipLevels = 1;
    texDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    texDesc.SampleDesc.Count = 1;

    D3D12_HEAP_PROPERTIES defaultHeap = HeapProps(D3D12_HEAP_TYPE_DEFAULT);
    ThrowIfFailed(m_device->CreateCommittedResource(
        &defaultHeap,
        D3D12_HEAP_FLAG_NONE,
        &texDesc,
        D3D12_RESOURCE_STATE_COPY_DEST,
        nullptr,
        IID_PPV_ARGS(&m_defaultTexture)));

    UINT64 uploadSize = 0;
    m_device->GetCopyableFootprints(&texDesc, 0, 1, 0, nullptr, nullptr, nullptr, &uploadSize);
    D3D12_HEAP_PROPERTIES uploadHeap = HeapProps(D3D12_HEAP_TYPE_UPLOAD);
    auto uploadDesc = BufferDesc(uploadSize);
    ComPtr<ID3D12Resource> upload;
    ThrowIfFailed(m_device->CreateCommittedResource(
        &uploadHeap,
        D3D12_HEAP_FLAG_NONE,
        &uploadDesc,
        D3D12_RESOURCE_STATE_GENERIC_READ,
        nullptr,
        IID_PPV_ARGS(&upload)));

    D3D12_SUBRESOURCE_DATA data = {};
    data.pData = &white;
    data.RowPitch = sizeof(uint32_t);
    data.SlicePitch = sizeof(uint32_t);

    ThrowIfFailed(m_frameResources[0].commandAllocator->Reset());
    ThrowIfFailed(m_commandList->Reset(m_frameResources[0].commandAllocator.Get(), nullptr));
    UploadTexture(m_device.Get(), m_commandList.Get(), m_defaultTexture.Get(), upload.Get(), data);

    D3D12_RESOURCE_BARRIER barrier = {};
    barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
    barrier.Transition.pResource = m_defaultTexture.Get();
    barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_COPY_DEST;
    barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE;
    barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
    m_commandList->ResourceBarrier(1, &barrier);

    ThrowIfFailed(m_commandList->Close());
    ID3D12CommandList *lists[] = {m_commandList.Get()};
    m_commandQueue->ExecuteCommandLists(1, lists);
    WaitForGpu();

    m_defaultSrv = m_srvHeap->GetCPUDescriptorHandleForHeapStart();
    D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
    srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
    srvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
    srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
    srvDesc.Texture2D.MipLevels = 1;
    m_device->CreateShaderResourceView(m_defaultTexture.Get(), &srvDesc, m_defaultSrv);
}

void Renderer::CreateRootSignature() {
    D3D12_DESCRIPTOR_RANGE range = {};
    range.RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
    range.NumDescriptors = 1;
    range.BaseShaderRegister = 0;
    range.RegisterSpace = 0;
    range.OffsetInDescriptorsFromTableStart = 0;

    D3D12_ROOT_PARAMETER params[2] = {};
    params[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
    params[0].Descriptor.ShaderRegister = 0;
    params[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;
    params[1].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
    params[1].DescriptorTable.NumDescriptorRanges = 1;
    params[1].DescriptorTable.pDescriptorRanges = &range;
    params[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;

    D3D12_STATIC_SAMPLER_DESC sampler = {};
    sampler.Filter = D3D12_FILTER_MIN_MAG_MIP_LINEAR;
    sampler.AddressU = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
    sampler.AddressV = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
    sampler.AddressW = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
    sampler.ShaderRegister = 0;
    sampler.RegisterSpace = 0;
    sampler.ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;

    D3D12_ROOT_SIGNATURE_DESC desc = {};
    desc.NumParameters = _countof(params);
    desc.pParameters = params;
    desc.NumStaticSamplers = 1;
    desc.pStaticSamplers = &sampler;
    desc.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;

    ComPtr<ID3DBlob> serialized;
    ComPtr<ID3DBlob> error;
    ThrowIfFailed(D3D12SerializeRootSignature(&desc, D3D_ROOT_SIGNATURE_VERSION_1, &serialized, &error));
    ThrowIfFailed(m_device->CreateRootSignature(0, serialized->GetBufferPointer(), serialized->GetBufferSize(), IID_PPV_ARGS(&m_rootSignature)));
}

void Renderer::CreatePipelineState() {
    auto vs = CompileShader(L"shaders/mesh.hlsl", "VSMain", "vs_5_1");
    auto ps = CompileShader(L"shaders/mesh.hlsl", "PSMain", "ps_5_1");

    D3D12_INPUT_ELEMENT_DESC layout[] = {
        {"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},
        {"NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},
        {"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 24, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},
    };

    D3D12_GRAPHICS_PIPELINE_STATE_DESC psoDesc = {};
    psoDesc.InputLayout = {layout, _countof(layout)};
    psoDesc.pRootSignature = m_rootSignature.Get();
    psoDesc.VS = {vs->GetBufferPointer(), vs->GetBufferSize()};
    psoDesc.PS = {ps->GetBufferPointer(), ps->GetBufferSize()};
    psoDesc.RasterizerState = DefaultRasterizer();
    psoDesc.BlendState = DefaultBlend();
    psoDesc.DepthStencilState = DefaultDepthStencil();
    psoDesc.SampleMask = UINT_MAX;
    psoDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
    psoDesc.NumRenderTargets = 1;
    psoDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
    psoDesc.DSVFormat = DXGI_FORMAT_D32_FLOAT;
    psoDesc.SampleDesc.Count = 1;

    ThrowIfFailed(m_device->CreateGraphicsPipelineState(&psoDesc, IID_PPV_ARGS(&m_pipelineState)));
}

void Renderer::CreateConstantBuffer() {
    for (auto &frame : m_frameResources) {
        D3D12_HEAP_PROPERTIES heapProps = HeapProps(D3D12_HEAP_TYPE_UPLOAD);
        auto bufferDesc = BufferDesc(sizeof(ObjectConstants) * 128);
        ThrowIfFailed(m_device->CreateCommittedResource(
            &heapProps,
            D3D12_HEAP_FLAG_NONE,
            &bufferDesc,
            D3D12_RESOURCE_STATE_GENERIC_READ,
            nullptr,
            IID_PPV_ARGS(&frame.constantBuffer)));

        D3D12_RANGE range = {0, 0};
        ThrowIfFailed(frame.constantBuffer->Map(0, &range, reinterpret_cast<void **>(&frame.mappedData)));
    }
}

void Renderer::Resize(UINT width, UINT height) {
    if (width == 0 || height == 0) {
        return;
    }

    WaitForGpu();

    for (auto &rt : m_renderTargets) {
        rt.Reset();
    }
    m_depthStencil.Reset();

    ThrowIfFailed(m_swapChain->ResizeBuffers(kFrameCount, width, height, DXGI_FORMAT_R8G8B8A8_UNORM, 0));
    m_frameIndex = m_swapChain->GetCurrentBackBufferIndex();
    m_width = width;
    m_height = height;

    CreateRenderTargets();
    CreateDepthStencil();
}

void Renderer::SetViewportSize(UINT width, UINT height) {
    Resize(width, height);
}

void Renderer::SetClearColor(const DirectX::XMFLOAT4 &color) {
    m_clearColor = color;
}

Renderer::FrameResource &Renderer::CurrentFrame() {
    return m_frameResources[m_frameIndex];
}

void Renderer::UpdateMeshBuffers(MeshObject &object, size_t index) {
    MeshGpuResources meshResources;
    if (object.mesh.vertices.empty() || object.mesh.indices.empty()) {
        return;
    }

    UINT vbSize = static_cast<UINT>(object.mesh.vertices.size() * sizeof(Vertex));
    UINT ibSize = static_cast<UINT>(object.mesh.indices.size() * sizeof(uint32_t));

    ComPtr<ID3D12Resource> vbUpload;
    D3D12_HEAP_PROPERTIES defaultHeap = HeapProps(D3D12_HEAP_TYPE_DEFAULT);
    D3D12_HEAP_PROPERTIES uploadHeap = HeapProps(D3D12_HEAP_TYPE_UPLOAD);
    auto vbDesc = BufferDesc(vbSize);
    ThrowIfFailed(m_device->CreateCommittedResource(
        &defaultHeap,
        D3D12_HEAP_FLAG_NONE,
        &vbDesc,
        D3D12_RESOURCE_STATE_COPY_DEST,
        nullptr,
        IID_PPV_ARGS(&meshResources.vertexBuffer)));

    ThrowIfFailed(m_device->CreateCommittedResource(
        &uploadHeap,
        D3D12_HEAP_FLAG_NONE,
        &vbDesc,
        D3D12_RESOURCE_STATE_GENERIC_READ,
        nullptr,
        IID_PPV_ARGS(&vbUpload)));

    UINT8 *vertexData;
    D3D12_RANGE range = {0, 0};
    ThrowIfFailed(vbUpload->Map(0, &range, reinterpret_cast<void **>(&vertexData)));
    memcpy(vertexData, object.mesh.vertices.data(), vbSize);
    vbUpload->Unmap(0, nullptr);

    ComPtr<ID3D12Resource> ibUpload;
    auto ibDesc = BufferDesc(ibSize);
    ThrowIfFailed(m_device->CreateCommittedResource(
        &defaultHeap,
        D3D12_HEAP_FLAG_NONE,
        &ibDesc,
        D3D12_RESOURCE_STATE_COPY_DEST,
        nullptr,
        IID_PPV_ARGS(&meshResources.indexBuffer)));

    ThrowIfFailed(m_device->CreateCommittedResource(
        &uploadHeap,
        D3D12_HEAP_FLAG_NONE,
        &ibDesc,
        D3D12_RESOURCE_STATE_GENERIC_READ,
        nullptr,
        IID_PPV_ARGS(&ibUpload)));

    UINT8 *indexData;
    ThrowIfFailed(ibUpload->Map(0, &range, reinterpret_cast<void **>(&indexData)));
    memcpy(indexData, object.mesh.indices.data(), ibSize);
    ibUpload->Unmap(0, nullptr);

    ThrowIfFailed(m_frameResources[0].commandAllocator->Reset());
    ThrowIfFailed(m_commandList->Reset(m_frameResources[0].commandAllocator.Get(), nullptr));
    m_commandList->CopyBufferRegion(meshResources.vertexBuffer.Get(), 0, vbUpload.Get(), 0, vbSize);
    m_commandList->CopyBufferRegion(meshResources.indexBuffer.Get(), 0, ibUpload.Get(), 0, ibSize);
    D3D12_RESOURCE_BARRIER vbBarrier = {};
    vbBarrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
    vbBarrier.Transition.pResource = meshResources.vertexBuffer.Get();
    vbBarrier.Transition.StateBefore = D3D12_RESOURCE_STATE_COPY_DEST;
    vbBarrier.Transition.StateAfter = D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER;
    vbBarrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
    m_commandList->ResourceBarrier(1, &vbBarrier);

    D3D12_RESOURCE_BARRIER ibBarrier = {};
    ibBarrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
    ibBarrier.Transition.pResource = meshResources.indexBuffer.Get();
    ibBarrier.Transition.StateBefore = D3D12_RESOURCE_STATE_COPY_DEST;
    ibBarrier.Transition.StateAfter = D3D12_RESOURCE_STATE_INDEX_BUFFER;
    ibBarrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
    m_commandList->ResourceBarrier(1, &ibBarrier);
    ThrowIfFailed(m_commandList->Close());

    ID3D12CommandList *lists[] = {m_commandList.Get()};
    m_commandQueue->ExecuteCommandLists(1, lists);
    WaitForGpu();

    meshResources.vbv.BufferLocation = meshResources.vertexBuffer->GetGPUVirtualAddress();
    meshResources.vbv.StrideInBytes = sizeof(Vertex);
    meshResources.vbv.SizeInBytes = vbSize;

    meshResources.ibv.BufferLocation = meshResources.indexBuffer->GetGPUVirtualAddress();
    meshResources.ibv.Format = DXGI_FORMAT_R32_UINT;
    meshResources.ibv.SizeInBytes = ibSize;
    meshResources.indexCount = static_cast<UINT>(object.mesh.indices.size());

    if (index >= m_objectStates.size()) {
        m_objectStates.resize(index + 1);
    }
    m_objectStates[index].mesh = meshResources;
}

void Renderer::UpdateTexture(MeshObject &object, size_t index) {
    if (object.material.texturePath.empty()) {
        return;
    }

    ComPtr<IWICImagingFactory> factory;
    ThrowIfFailed(CoCreateInstance(CLSID_WICImagingFactory, nullptr, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&factory)));

    ComPtr<IWICBitmapDecoder> decoder;
    ThrowIfFailed(factory->CreateDecoderFromFilename(object.material.texturePath.c_str(), nullptr, GENERIC_READ, WICDecodeMetadataCacheOnLoad, &decoder));

    ComPtr<IWICBitmapFrameDecode> frame;
    ThrowIfFailed(decoder->GetFrame(0, &frame));

    ComPtr<IWICFormatConverter> converter;
    ThrowIfFailed(factory->CreateFormatConverter(&converter));
    ThrowIfFailed(converter->Initialize(frame.Get(), GUID_WICPixelFormat32bppRGBA, WICBitmapDitherTypeNone, nullptr, 0.0, WICBitmapPaletteTypeCustom));

    UINT width = 0;
    UINT height = 0;
    ThrowIfFailed(converter->GetSize(&width, &height));
    std::vector<uint8_t> pixels(width * height * 4);
    ThrowIfFailed(converter->CopyPixels(nullptr, width * 4, static_cast<UINT>(pixels.size()), pixels.data()));

    D3D12_RESOURCE_DESC texDesc = {};
    texDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
    texDesc.Width = width;
    texDesc.Height = height;
    texDesc.DepthOrArraySize = 1;
    texDesc.MipLevels = 1;
    texDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    texDesc.SampleDesc.Count = 1;

    ComPtr<ID3D12Resource> texture;
    D3D12_HEAP_PROPERTIES defaultHeap = HeapProps(D3D12_HEAP_TYPE_DEFAULT);
    ThrowIfFailed(m_device->CreateCommittedResource(
        &defaultHeap,
        D3D12_HEAP_FLAG_NONE,
        &texDesc,
        D3D12_RESOURCE_STATE_COPY_DEST,
        nullptr,
        IID_PPV_ARGS(&texture)));

    UINT64 uploadSize = 0;
    m_device->GetCopyableFootprints(&texDesc, 0, 1, 0, nullptr, nullptr, nullptr, &uploadSize);

    ComPtr<ID3D12Resource> upload;
    D3D12_HEAP_PROPERTIES uploadHeap = HeapProps(D3D12_HEAP_TYPE_UPLOAD);
    auto uploadDesc = BufferDesc(uploadSize);
    ThrowIfFailed(m_device->CreateCommittedResource(
        &uploadHeap,
        D3D12_HEAP_FLAG_NONE,
        &uploadDesc,
        D3D12_RESOURCE_STATE_GENERIC_READ,
        nullptr,
        IID_PPV_ARGS(&upload)));

    D3D12_SUBRESOURCE_DATA subresource = {};
    subresource.pData = pixels.data();
    subresource.RowPitch = width * 4;
    subresource.SlicePitch = subresource.RowPitch * height;

    ThrowIfFailed(m_frameResources[0].commandAllocator->Reset());
    ThrowIfFailed(m_commandList->Reset(m_frameResources[0].commandAllocator.Get(), nullptr));
    UploadTexture(m_device.Get(), m_commandList.Get(), texture.Get(), upload.Get(), subresource);
    D3D12_RESOURCE_BARRIER texBarrier = {};
    texBarrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
    texBarrier.Transition.pResource = texture.Get();
    texBarrier.Transition.StateBefore = D3D12_RESOURCE_STATE_COPY_DEST;
    texBarrier.Transition.StateAfter = D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE;
    texBarrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
    m_commandList->ResourceBarrier(1, &texBarrier);
    ThrowIfFailed(m_commandList->Close());
    ID3D12CommandList *lists[] = {m_commandList.Get()};
    m_commandQueue->ExecuteCommandLists(1, lists);
    WaitForGpu();

    D3D12_CPU_DESCRIPTOR_HANDLE srvHandle = m_srvHeap->GetCPUDescriptorHandleForHeapStart();
    srvHandle.ptr += static_cast<SIZE_T>(index + 1) * m_srvDescriptorSize;

    D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
    srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
    srvDesc.Format = GetSRGBFormat(texDesc.Format);
    srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
    srvDesc.Texture2D.MipLevels = 1;

    m_device->CreateShaderResourceView(texture.Get(), &srvDesc, srvHandle);

    if (index >= m_objectStates.size()) {
        m_objectStates.resize(index + 1);
    }
    m_objectStates[index].texture.texture = texture;
    m_objectStates[index].texture.srvHandle = srvHandle;
    m_objectStates[index].texture.valid = true;
    object.material.hasTexture = true;
}

void Renderer::Render(const std::vector<std::unique_ptr<MeshObject>> &objects, int selectedIndex, const Camera &camera) {
    if (!m_device) {
        return;
    }

    if (m_objectStates.size() != objects.size()) {
        m_objectStates.resize(objects.size());
        for (size_t i = 0; i < objects.size(); ++i) {
            UpdateMeshBuffers(*objects[i], i);
            if (!objects[i]->material.texturePath.empty()) {
                UpdateTexture(*objects[i], i);
            }
        }
    }

    auto &frame = CurrentFrame();
    ThrowIfFailed(frame.commandAllocator->Reset());
    ThrowIfFailed(m_commandList->Reset(frame.commandAllocator.Get(), m_pipelineState.Get()));

    D3D12_VIEWPORT viewport = {0.0f, 0.0f, static_cast<float>(m_width), static_cast<float>(m_height), 0.0f, 1.0f};
    D3D12_RECT scissor = {0, 0, static_cast<LONG>(m_width), static_cast<LONG>(m_height)};
    m_commandList->RSSetViewports(1, &viewport);
    m_commandList->RSSetScissorRects(1, &scissor);

    D3D12_RESOURCE_BARRIER barrier = {};
    barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
    barrier.Transition.pResource = m_renderTargets[m_frameIndex].Get();
    barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_PRESENT;
    barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_RENDER_TARGET;
    barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
    m_commandList->ResourceBarrier(1, &barrier);

    D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle = OffsetCpuHandle(m_rtvHeap->GetCPUDescriptorHandleForHeapStart(), m_frameIndex, m_rtvDescriptorSize);
    D3D12_CPU_DESCRIPTOR_HANDLE dsvHandle = m_dsvHeap->GetCPUDescriptorHandleForHeapStart();

    float clearColor[4] = {m_clearColor.x, m_clearColor.y, m_clearColor.z, m_clearColor.w};
    m_commandList->ClearRenderTargetView(rtvHandle, clearColor, 0, nullptr);
    m_commandList->ClearDepthStencilView(dsvHandle, D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0, 0, nullptr);
    m_commandList->OMSetRenderTargets(1, &rtvHandle, FALSE, &dsvHandle);

    m_commandList->SetGraphicsRootSignature(m_rootSignature.Get());
    ID3D12DescriptorHeap *heaps[] = {m_srvHeap.Get()};
    m_commandList->SetDescriptorHeaps(1, heaps);

    XMMATRIX view = XMMatrixLookAtLH(XMLoadFloat3(&camera.position), XMLoadFloat3(&camera.target), XMVectorSet(0, 1, 0, 0));
    XMMATRIX proj = XMMatrixPerspectiveFovLH(camera.fov, static_cast<float>(m_width) / static_cast<float>(m_height), camera.nearPlane, camera.farPlane);

    for (size_t i = 0; i < objects.size(); ++i) {
        const auto &object = objects[i];
        auto &gpu = m_objectStates[i];
        if (gpu.mesh.indexCount == 0) {
            continue;
        }

        XMMATRIX scale = XMMatrixScaling(object->transform.scale.x, object->transform.scale.y, object->transform.scale.z);
        XMMATRIX rot = XMMatrixRotationRollPitchYaw(object->transform.rotation.x, object->transform.rotation.y, object->transform.rotation.z);
        XMMATRIX trans = XMMatrixTranslation(object->transform.position.x, object->transform.position.y, object->transform.position.z);
        XMMATRIX model = scale * rot * trans;

        ObjectConstants constants{};
        XMStoreFloat4x4(&constants.model, XMMatrixTranspose(model));
        XMStoreFloat4x4(&constants.view, XMMatrixTranspose(view));
        XMStoreFloat4x4(&constants.proj, XMMatrixTranspose(proj));
        constants.color = object->material.baseColor;
        constants.useTexture = object->material.hasTexture ? 1 : 0;

        memcpy(frame.mappedData + i * sizeof(ObjectConstants), &constants, sizeof(ObjectConstants));

        D3D12_GPU_VIRTUAL_ADDRESS cbAddress = frame.constantBuffer->GetGPUVirtualAddress() + i * sizeof(ObjectConstants);
        m_commandList->SetGraphicsRootConstantBufferView(0, cbAddress);

        D3D12_GPU_DESCRIPTOR_HANDLE handle = m_srvHeap->GetGPUDescriptorHandleForHeapStart();
        if (gpu.texture.valid) {
            handle = OffsetGpuHandle(handle, static_cast<UINT>(i + 1), m_srvDescriptorSize);
        }
        m_commandList->SetGraphicsRootDescriptorTable(1, handle);

        m_commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
        m_commandList->IASetVertexBuffers(0, 1, &gpu.mesh.vbv);
        m_commandList->IASetIndexBuffer(&gpu.mesh.ibv);
        m_commandList->DrawIndexedInstanced(gpu.mesh.indexCount, 1, 0, 0, 0);
    }

    barrier = {};
    barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
    barrier.Transition.pResource = m_renderTargets[m_frameIndex].Get();
    barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET;
    barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_PRESENT;
    barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
    m_commandList->ResourceBarrier(1, &barrier);

    ThrowIfFailed(m_commandList->Close());
    ID3D12CommandList *lists[] = {m_commandList.Get()};
    m_commandQueue->ExecuteCommandLists(1, lists);

    ThrowIfFailed(m_swapChain->Present(1, 0));
    MoveToNextFrame();
}

void Renderer::WaitForGpu() {
    auto &frame = CurrentFrame();
    frame.fenceValue++;
    ThrowIfFailed(m_commandQueue->Signal(m_fence.Get(), frame.fenceValue));
    ThrowIfFailed(m_fence->SetEventOnCompletion(frame.fenceValue, m_fenceEvent));
    WaitForSingleObject(m_fenceEvent, INFINITE);
}

void Renderer::MoveToNextFrame() {
    auto &frame = CurrentFrame();
    const UINT64 currentFence = frame.fenceValue;
    ThrowIfFailed(m_commandQueue->Signal(m_fence.Get(), currentFence));

    m_frameIndex = m_swapChain->GetCurrentBackBufferIndex();
    auto &nextFrame = CurrentFrame();
    if (m_fence->GetCompletedValue() < nextFrame.fenceValue) {
        ThrowIfFailed(m_fence->SetEventOnCompletion(nextFrame.fenceValue, m_fenceEvent));
        WaitForSingleObject(m_fenceEvent, INFINITE);
    }

    nextFrame.fenceValue = currentFence + 1;
}
