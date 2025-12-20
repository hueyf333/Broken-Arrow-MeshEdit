#include <windows.h>
#include <windowsx.h>
#include <commctrl.h>
#include <d3d11.h>
#include <d3dcompiler.h>
#include <DirectXMath.h>

#include <algorithm>
#include <array>
#include <chrono>
#include <cstdint>
#include <cmath>
#include <string>
#include <thread>
#include <vector>

#pragma comment(lib, "Comctl32.lib")

using DirectX::XMFLOAT2;
using DirectX::XMFLOAT3;
using DirectX::XMFLOAT4;
using DirectX::XMMATRIX;
using DirectX::XMMatrixLookAtLH;
using DirectX::XMMatrixPerspectiveFovLH;
using DirectX::XMMatrixRotationRollPitchYaw;
using DirectX::XMMatrixScaling;
using DirectX::XMMatrixTranslation;
using DirectX::XMMatrixTranspose;
using DirectX::XMVectorSet;

namespace
{
    constexpr wchar_t kMainWindowClass[] = L"BrokenArrowMeshEdit.Main";
    constexpr wchar_t kPanelWindowClass[] = L"BrokenArrowMeshEdit.Panel";
    constexpr wchar_t kViewportWindowClass[] = L"BrokenArrowMeshEdit.Viewport";

    constexpr UINT kTimerId = 1;
    constexpr int kToolbarWidth = 72;
    constexpr int kOutlinerWidth = 260;
    constexpr int kPropertiesWidth = 320;
    constexpr int kTimelineHeight = 180;

    enum class DockArea
    {
        Left,
        Right,
        Bottom,
    };

    enum class PrimitiveType
    {
        Cube,
        Sphere,
        Grid,
    };

    struct Vertex
    {
        XMFLOAT3 position;
        XMFLOAT3 color;
    };

    struct SceneConstants
    {
        XMFLOAT4X4 world;
        XMFLOAT4X4 view;
        XMFLOAT4X4 projection;
        XMFLOAT4 tint;
    };

    struct Mesh
    {
        std::vector<Vertex> vertices;
        std::vector<uint32_t> indices;
    };

    struct Panel
    {
        std::wstring title;
        DockArea dockArea{};
        HWND hwnd = nullptr;
        bool docked = true;
        RECT dockRect{};
    };

    struct Camera
    {
        float distance = 6.5f;
        float yaw = 0.6f;
        float pitch = 0.35f;
        XMFLOAT3 target{0.0f, 0.5f, 0.0f};

        XMMATRIX GetViewMatrix() const
        {
            XMMATRIX rotation = XMMatrixRotationRollPitchYaw(pitch, yaw, 0.0f);
            auto offset = DirectX::XMVector3TransformCoord(XMVectorSet(0.0f, 0.0f, distance, 0.0f), rotation);
            auto targetVec = DirectX::XMLoadFloat3(&target);
            auto position = DirectX::XMVectorAdd(targetVec, offset);
            return XMMatrixLookAtLH(position, targetVec, XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f));
        }
    };

    struct MeshResources
    {
        ID3D11Buffer* vertexBuffer = nullptr;
        ID3D11Buffer* indexBuffer = nullptr;
        UINT indexCount = 0;
    };

    struct GridResources
    {
        ID3D11Buffer* vertexBuffer = nullptr;
        UINT vertexCount = 0;
    };

    struct D3DResources
    {
        ID3D11Device* device = nullptr;
        ID3D11DeviceContext* context = nullptr;
        IDXGISwapChain* swapChain = nullptr;
        ID3D11RenderTargetView* rtv = nullptr;
        ID3D11DepthStencilView* dsv = nullptr;
        ID3D11Texture2D* depthBuffer = nullptr;
        D3D11_VIEWPORT viewport{};

        ID3D11VertexShader* vertexShader = nullptr;
        ID3D11PixelShader* pixelShader = nullptr;
        ID3D11InputLayout* inputLayout = nullptr;
        ID3D11Buffer* constantBuffer = nullptr;
        ID3D11RasterizerState* solidState = nullptr;
        ID3D11RasterizerState* wireframeState = nullptr;
    };

    struct AppState
    {
        HWND mainWindow = nullptr;
        HWND viewportWindow = nullptr;
        HFONT uiFont = nullptr;
        HBRUSH panelBrush = nullptr;
        HBRUSH panelHeaderBrush = nullptr;
        COLORREF panelTextColor = RGB(220, 220, 220);

        Panel toolbar{"Tool Shelf", DockArea::Left};
        Panel outliner{"Outliner", DockArea::Right};
        Panel properties{"Properties", DockArea::Right};
        Panel timeline{"Timeline", DockArea::Bottom};

        D3DResources d3d{};
        MeshResources mesh{};
        GridResources grid{};
        Camera camera{};

        PrimitiveType primitive = PrimitiveType::Cube;
        bool showGrid = true;
        bool wireframe = false;
        bool orbiting = false;
        bool panning = false;
        POINT lastMouse{};
        XMFLOAT3 meshPosition{0.0f, 0.0f, 0.0f};
        XMFLOAT3 meshRotation{0.0f, 0.0f, 0.0f};
        XMFLOAT3 meshScale{1.0f, 1.0f, 1.0f};
        int sphereSegments = 24;
        int sphereRings = 16;
    };

    AppState gApp;

    std::wstring ToWide(const std::string& value)
    {
        if (value.empty())
        {
            return {};
        }
        int length = MultiByteToWideChar(CP_UTF8, 0, value.c_str(), -1, nullptr, 0);
        std::wstring output(length - 1, L'\0');
        MultiByteToWideChar(CP_UTF8, 0, value.c_str(), -1, output.data(), length);
        return output;
    }

    Mesh CreateCubeMesh()
    {
        Mesh mesh;
        mesh.vertices = {
            {{-1.0f, -1.0f, -1.0f}, {0.8f, 0.2f, 0.2f}},
            {{-1.0f,  1.0f, -1.0f}, {0.8f, 0.4f, 0.2f}},
            {{ 1.0f,  1.0f, -1.0f}, {0.8f, 0.6f, 0.2f}},
            {{ 1.0f, -1.0f, -1.0f}, {0.8f, 0.8f, 0.2f}},
            {{-1.0f, -1.0f,  1.0f}, {0.2f, 0.6f, 0.9f}},
            {{-1.0f,  1.0f,  1.0f}, {0.3f, 0.7f, 0.9f}},
            {{ 1.0f,  1.0f,  1.0f}, {0.4f, 0.8f, 0.9f}},
            {{ 1.0f, -1.0f,  1.0f}, {0.5f, 0.9f, 0.9f}},
        };

        mesh.indices = {
            0, 1, 2, 0, 2, 3,
            4, 6, 5, 4, 7, 6,
            4, 5, 1, 4, 1, 0,
            3, 2, 6, 3, 6, 7,
            1, 5, 6, 1, 6, 2,
            4, 0, 3, 4, 3, 7,
        };
        return mesh;
    }

    Mesh CreateGridMesh()
    {
        Mesh mesh;
        mesh.vertices = {
            {{-1.0f, 0.0f, -1.0f}, {0.4f, 0.6f, 0.8f}},
            {{-1.0f, 0.0f,  1.0f}, {0.4f, 0.6f, 0.8f}},
            {{ 1.0f, 0.0f,  1.0f}, {0.4f, 0.6f, 0.8f}},
            {{ 1.0f, 0.0f, -1.0f}, {0.4f, 0.6f, 0.8f}},
        };

        mesh.indices = {0, 1, 2, 0, 2, 3};
        return mesh;
    }

    Mesh CreateSphereMesh(int segments, int rings)
    {
        Mesh mesh;
        segments = std::clamp(segments, 8, 64);
        rings = std::clamp(rings, 6, 48);

        for (int ring = 0; ring <= rings; ++ring)
        {
            float v = static_cast<float>(ring) / static_cast<float>(rings);
            float phi = v * DirectX::XM_PI;
            float y = std::cos(phi);
            float r = std::sin(phi);

            for (int seg = 0; seg <= segments; ++seg)
            {
                float u = static_cast<float>(seg) / static_cast<float>(segments);
                float theta = u * DirectX::XM_2PI;
                float x = r * std::cos(theta);
                float z = r * std::sin(theta);
                mesh.vertices.push_back({{x, y, z}, {0.6f, 0.7f, 0.9f}});
            }
        }

        int stride = segments + 1;
        for (int ring = 0; ring < rings; ++ring)
        {
            for (int seg = 0; seg < segments; ++seg)
            {
                int a = ring * stride + seg;
                int b = a + stride;
                int c = b + 1;
                int d = a + 1;
                mesh.indices.push_back(a);
                mesh.indices.push_back(b);
                mesh.indices.push_back(c);
                mesh.indices.push_back(a);
                mesh.indices.push_back(c);
                mesh.indices.push_back(d);
            }
        }

        return mesh;
    }

    bool CreateBuffer(ID3D11Device* device, const void* data, UINT size, UINT bindFlags, ID3D11Buffer** buffer)
    {
        if (!device || !data || size == 0 || !buffer)
        {
            return false;
        }
        D3D11_BUFFER_DESC desc{};
        desc.ByteWidth = size;
        desc.Usage = D3D11_USAGE_DEFAULT;
        desc.BindFlags = bindFlags;

        D3D11_SUBRESOURCE_DATA initData{};
        initData.pSysMem = data;

        return SUCCEEDED(device->CreateBuffer(&desc, &initData, buffer));
    }

    void ReleaseMeshResources(MeshResources& resources)
    {
        if (resources.vertexBuffer)
        {
            resources.vertexBuffer->Release();
            resources.vertexBuffer = nullptr;
        }
        if (resources.indexBuffer)
        {
            resources.indexBuffer->Release();
            resources.indexBuffer = nullptr;
        }
        resources.indexCount = 0;
    }

    void ReleaseGridResources(GridResources& resources)
    {
        if (resources.vertexBuffer)
        {
            resources.vertexBuffer->Release();
            resources.vertexBuffer = nullptr;
        }
        resources.vertexCount = 0;
    }

    void UpdateMeshResources(const Mesh& mesh)
    {
        ReleaseMeshResources(gApp.mesh);

        if (!mesh.vertices.empty())
        {
            CreateBuffer(gApp.d3d.device, mesh.vertices.data(),
                static_cast<UINT>(mesh.vertices.size() * sizeof(Vertex)),
                D3D11_BIND_VERTEX_BUFFER,
                &gApp.mesh.vertexBuffer);
        }
        if (!mesh.indices.empty())
        {
            CreateBuffer(gApp.d3d.device, mesh.indices.data(),
                static_cast<UINT>(mesh.indices.size() * sizeof(uint32_t)),
                D3D11_BIND_INDEX_BUFFER,
                &gApp.mesh.indexBuffer);
            gApp.mesh.indexCount = static_cast<UINT>(mesh.indices.size());
        }
    }

    void UpdateGridResources()
    {
        ReleaseGridResources(gApp.grid);
        std::vector<Vertex> lines;
        const int gridSize = 20;
        const float spacing = 0.5f;
        for (int i = -gridSize; i <= gridSize; ++i)
        {
            float offset = static_cast<float>(i) * spacing;
            XMFLOAT3 color = (i == 0) ? XMFLOAT3{0.9f, 0.4f, 0.4f} : XMFLOAT3{0.2f, 0.25f, 0.3f};
            lines.push_back({{-gridSize * spacing, 0.0f, offset}, color});
            lines.push_back({{ gridSize * spacing, 0.0f, offset}, color});
            color = (i == 0) ? XMFLOAT3{0.4f, 0.8f, 0.4f} : XMFLOAT3{0.2f, 0.25f, 0.3f};
            lines.push_back({{offset, 0.0f, -gridSize * spacing}, color});
            lines.push_back({{offset, 0.0f,  gridSize * spacing}, color});
        }

        if (!lines.empty())
        {
            CreateBuffer(gApp.d3d.device, lines.data(),
                static_cast<UINT>(lines.size() * sizeof(Vertex)),
                D3D11_BIND_VERTEX_BUFFER,
                &gApp.grid.vertexBuffer);
            gApp.grid.vertexCount = static_cast<UINT>(lines.size());
        }
    }

    void CreateDefaultMesh()
    {
        switch (gApp.primitive)
        {
        case PrimitiveType::Cube:
            UpdateMeshResources(CreateCubeMesh());
            break;
        case PrimitiveType::Sphere:
            UpdateMeshResources(CreateSphereMesh(gApp.sphereSegments, gApp.sphereRings));
            break;
        case PrimitiveType::Grid:
            UpdateMeshResources(CreateGridMesh());
            break;
        }
    }

    std::string GetPrimitiveName(PrimitiveType type)
    {
        switch (type)
        {
        case PrimitiveType::Cube:
            return "Cube";
        case PrimitiveType::Sphere:
            return "Sphere";
        case PrimitiveType::Grid:
            return "Grid";
        }
        return "Unknown";
    }

    void CreateDeviceResources(HWND hwnd)
    {
        DXGI_SWAP_CHAIN_DESC swapDesc{};
        swapDesc.BufferCount = 2;
        swapDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
        swapDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
        swapDesc.OutputWindow = hwnd;
        swapDesc.SampleDesc.Count = 1;
        swapDesc.Windowed = TRUE;
        swapDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

        UINT flags = 0;
#if defined(_DEBUG)
        flags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

        D3D_FEATURE_LEVEL featureLevel{};
        if (FAILED(D3D11CreateDeviceAndSwapChain(
                nullptr,
                D3D_DRIVER_TYPE_HARDWARE,
                nullptr,
                flags,
                nullptr,
                0,
                D3D11_SDK_VERSION,
                &swapDesc,
                &gApp.d3d.swapChain,
                &gApp.d3d.device,
                &featureLevel,
                &gApp.d3d.context)))
        {
            MessageBox(hwnd, L"Failed to create DirectX 11 device.", L"MeshEdit", MB_OK | MB_ICONERROR);
        }
    }

    void ReleaseRenderTargets()
    {
        if (gApp.d3d.rtv)
        {
            gApp.d3d.rtv->Release();
            gApp.d3d.rtv = nullptr;
        }
        if (gApp.d3d.dsv)
        {
            gApp.d3d.dsv->Release();
            gApp.d3d.dsv = nullptr;
        }
        if (gApp.d3d.depthBuffer)
        {
            gApp.d3d.depthBuffer->Release();
            gApp.d3d.depthBuffer = nullptr;
        }
    }

    void CreateRenderTargets()
    {
        ReleaseRenderTargets();

        ID3D11Texture2D* backBuffer = nullptr;
        if (FAILED(gApp.d3d.swapChain->GetBuffer(0, IID_PPV_ARGS(&backBuffer))))
        {
            return;
        }
        gApp.d3d.device->CreateRenderTargetView(backBuffer, nullptr, &gApp.d3d.rtv);
        backBuffer->Release();

        RECT rect{};
        GetClientRect(gApp.viewportWindow, &rect);
        UINT width = std::max<UINT>(1, rect.right - rect.left);
        UINT height = std::max<UINT>(1, rect.bottom - rect.top);

        D3D11_TEXTURE2D_DESC depthDesc{};
        depthDesc.Width = width;
        depthDesc.Height = height;
        depthDesc.MipLevels = 1;
        depthDesc.ArraySize = 1;
        depthDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
        depthDesc.SampleDesc.Count = 1;
        depthDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
        gApp.d3d.device->CreateTexture2D(&depthDesc, nullptr, &gApp.d3d.depthBuffer);
        gApp.d3d.device->CreateDepthStencilView(gApp.d3d.depthBuffer, nullptr, &gApp.d3d.dsv);

        gApp.d3d.viewport.TopLeftX = 0.0f;
        gApp.d3d.viewport.TopLeftY = 0.0f;
        gApp.d3d.viewport.Width = static_cast<float>(width);
        gApp.d3d.viewport.Height = static_cast<float>(height);
        gApp.d3d.viewport.MinDepth = 0.0f;
        gApp.d3d.viewport.MaxDepth = 1.0f;
    }

    void CreateShaders()
    {
        const char* shaderCode = R"(
cbuffer Scene : register(b0)
{
    float4x4 world;
    float4x4 view;
    float4x4 projection;
    float4 tint;
};

struct VSInput
{
    float3 pos : POSITION;
    float3 color : COLOR;
};

struct VSOutput
{
    float4 pos : SV_POSITION;
    float3 color : COLOR;
};

VSOutput VSMain(VSInput input)
{
    VSOutput output;
    float4 worldPos = mul(float4(input.pos, 1.0f), world);
    float4 viewPos = mul(worldPos, view);
    output.pos = mul(viewPos, projection);
    output.color = input.color * tint.rgb;
    return output;
}

float4 PSMain(VSOutput input) : SV_TARGET
{
    return float4(input.color, 1.0f);
}
)";

        ID3DBlob* vsBlob = nullptr;
        ID3DBlob* psBlob = nullptr;
        ID3DBlob* errorBlob = nullptr;

        if (FAILED(D3DCompile(shaderCode, strlen(shaderCode), nullptr, nullptr, nullptr,
                "VSMain", "vs_5_0", 0, 0, &vsBlob, &errorBlob)))
        {
            if (errorBlob)
            {
                std::string message(reinterpret_cast<const char*>(errorBlob->GetBufferPointer()), errorBlob->GetBufferSize());
                MessageBoxA(gApp.mainWindow, message.c_str(), "Shader Error", MB_OK | MB_ICONERROR);
                errorBlob->Release();
            }
            return;
        }

        if (FAILED(D3DCompile(shaderCode, strlen(shaderCode), nullptr, nullptr, nullptr,
                "PSMain", "ps_5_0", 0, 0, &psBlob, &errorBlob)))
        {
            if (errorBlob)
            {
                std::string message(reinterpret_cast<const char*>(errorBlob->GetBufferPointer()), errorBlob->GetBufferSize());
                MessageBoxA(gApp.mainWindow, message.c_str(), "Shader Error", MB_OK | MB_ICONERROR);
                errorBlob->Release();
            }
            if (vsBlob)
            {
                vsBlob->Release();
            }
            return;
        }

        gApp.d3d.device->CreateVertexShader(vsBlob->GetBufferPointer(), vsBlob->GetBufferSize(), nullptr, &gApp.d3d.vertexShader);
        gApp.d3d.device->CreatePixelShader(psBlob->GetBufferPointer(), psBlob->GetBufferSize(), nullptr, &gApp.d3d.pixelShader);

        D3D11_INPUT_ELEMENT_DESC layoutDesc[] = {
            {"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
            {"COLOR", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0},
        };
        gApp.d3d.device->CreateInputLayout(layoutDesc, 2, vsBlob->GetBufferPointer(), vsBlob->GetBufferSize(), &gApp.d3d.inputLayout);

        vsBlob->Release();
        psBlob->Release();

        D3D11_BUFFER_DESC cbDesc{};
        cbDesc.ByteWidth = sizeof(SceneConstants);
        cbDesc.Usage = D3D11_USAGE_DEFAULT;
        cbDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
        gApp.d3d.device->CreateBuffer(&cbDesc, nullptr, &gApp.d3d.constantBuffer);

        D3D11_RASTERIZER_DESC rsDesc{};
        rsDesc.FillMode = D3D11_FILL_SOLID;
        rsDesc.CullMode = D3D11_CULL_BACK;
        rsDesc.DepthClipEnable = TRUE;
        gApp.d3d.device->CreateRasterizerState(&rsDesc, &gApp.d3d.solidState);

        rsDesc.FillMode = D3D11_FILL_WIREFRAME;
        gApp.d3d.device->CreateRasterizerState(&rsDesc, &gApp.d3d.wireframeState);
    }

    void UpdateSwapChain()
    {
        if (!gApp.d3d.swapChain)
        {
            return;
        }
        gApp.d3d.context->OMSetRenderTargets(0, nullptr, nullptr);
        ReleaseRenderTargets();
        gApp.d3d.swapChain->ResizeBuffers(0, 0, 0, DXGI_FORMAT_UNKNOWN, 0);
        CreateRenderTargets();
    }

    void RenderScene()
    {
        if (!gApp.d3d.context || !gApp.d3d.rtv)
        {
            return;
        }

        float clearColor[4] = {0.07f, 0.08f, 0.1f, 1.0f};
        gApp.d3d.context->OMSetRenderTargets(1, &gApp.d3d.rtv, gApp.d3d.dsv);
        gApp.d3d.context->ClearRenderTargetView(gApp.d3d.rtv, clearColor);
        if (gApp.d3d.dsv)
        {
            gApp.d3d.context->ClearDepthStencilView(gApp.d3d.dsv, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
        }

        gApp.d3d.context->RSSetViewports(1, &gApp.d3d.viewport);
        gApp.d3d.context->IASetInputLayout(gApp.d3d.inputLayout);
        gApp.d3d.context->VSSetShader(gApp.d3d.vertexShader, nullptr, 0);
        gApp.d3d.context->PSSetShader(gApp.d3d.pixelShader, nullptr, 0);
        gApp.d3d.context->RSSetState(gApp.wireframe ? gApp.d3d.wireframeState : gApp.d3d.solidState);

        SceneConstants constants{};
        XMMATRIX world = XMMatrixScaling(gApp.meshScale.x, gApp.meshScale.y, gApp.meshScale.z)
            * XMMatrixRotationRollPitchYaw(gApp.meshRotation.x, gApp.meshRotation.y, gApp.meshRotation.z)
            * XMMatrixTranslation(gApp.meshPosition.x, gApp.meshPosition.y, gApp.meshPosition.z);
        XMMATRIX view = gApp.camera.GetViewMatrix();

        RECT rect{};
        GetClientRect(gApp.viewportWindow, &rect);
        float width = static_cast<float>(std::max<long>(1, rect.right - rect.left));
        float height = static_cast<float>(std::max<long>(1, rect.bottom - rect.top));
        XMMATRIX projection = XMMatrixPerspectiveFovLH(DirectX::XM_PIDIV4, width / height, 0.1f, 100.0f);

        XMStoreFloat4x4(&constants.world, XMMatrixTranspose(world));
        XMStoreFloat4x4(&constants.view, XMMatrixTranspose(view));
        XMStoreFloat4x4(&constants.projection, XMMatrixTranspose(projection));
        constants.tint = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);

        gApp.d3d.context->UpdateSubresource(gApp.d3d.constantBuffer, 0, nullptr, &constants, 0, 0);
        gApp.d3d.context->VSSetConstantBuffers(0, 1, &gApp.d3d.constantBuffer);
        gApp.d3d.context->PSSetConstantBuffers(0, 1, &gApp.d3d.constantBuffer);

        UINT stride = sizeof(Vertex);
        UINT offset = 0;

        if (gApp.showGrid && gApp.grid.vertexBuffer)
        {
            gApp.d3d.context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_LINELIST);
            gApp.d3d.context->IASetVertexBuffers(0, 1, &gApp.grid.vertexBuffer, &stride, &offset);
            gApp.d3d.context->Draw(gApp.grid.vertexCount, 0);
        }

        if (gApp.mesh.vertexBuffer && gApp.mesh.indexBuffer)
        {
            gApp.d3d.context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
            gApp.d3d.context->IASetVertexBuffers(0, 1, &gApp.mesh.vertexBuffer, &stride, &offset);
            gApp.d3d.context->IASetIndexBuffer(gApp.mesh.indexBuffer, DXGI_FORMAT_R32_UINT, 0);
            gApp.d3d.context->DrawIndexed(gApp.mesh.indexCount, 0, 0);
        }

        gApp.d3d.swapChain->Present(1, 0);
    }

    void DestroyDeviceResources()
    {
        ReleaseMeshResources(gApp.mesh);
        ReleaseGridResources(gApp.grid);
        ReleaseRenderTargets();

        if (gApp.d3d.constantBuffer)
        {
            gApp.d3d.constantBuffer->Release();
            gApp.d3d.constantBuffer = nullptr;
        }
        if (gApp.d3d.inputLayout)
        {
            gApp.d3d.inputLayout->Release();
            gApp.d3d.inputLayout = nullptr;
        }
        if (gApp.d3d.vertexShader)
        {
            gApp.d3d.vertexShader->Release();
            gApp.d3d.vertexShader = nullptr;
        }
        if (gApp.d3d.pixelShader)
        {
            gApp.d3d.pixelShader->Release();
            gApp.d3d.pixelShader = nullptr;
        }
        if (gApp.d3d.solidState)
        {
            gApp.d3d.solidState->Release();
            gApp.d3d.solidState = nullptr;
        }
        if (gApp.d3d.wireframeState)
        {
            gApp.d3d.wireframeState->Release();
            gApp.d3d.wireframeState = nullptr;
        }
        if (gApp.d3d.swapChain)
        {
            gApp.d3d.swapChain->Release();
            gApp.d3d.swapChain = nullptr;
        }
        if (gApp.d3d.context)
        {
            gApp.d3d.context->Release();
            gApp.d3d.context = nullptr;
        }
        if (gApp.d3d.device)
        {
            gApp.d3d.device->Release();
            gApp.d3d.device = nullptr;
        }
    }

    void UpdateDockLayout(HWND hwnd)
    {
        RECT rect{};
        GetClientRect(hwnd, &rect);
        int width = rect.right - rect.left;
        int height = rect.bottom - rect.top;

        int leftWidth = gApp.toolbar.docked ? kToolbarWidth : 0;
        int rightWidth = 0;
        if (gApp.outliner.docked || gApp.properties.docked)
        {
            rightWidth = std::max(kOutlinerWidth, kPropertiesWidth);
        }
        int bottomHeight = gApp.timeline.docked ? kTimelineHeight : 0;

        int viewportX = leftWidth;
        int viewportY = 0;
        int viewportWidth = std::max(1, width - leftWidth - rightWidth);
        int viewportHeight = std::max(1, height - bottomHeight);

        if (gApp.toolbar.docked)
        {
            MoveWindow(gApp.toolbar.hwnd, 0, 0, leftWidth, height - bottomHeight, TRUE);
        }

        int rightX = width - rightWidth;
        int rightY = 0;
        if (gApp.outliner.docked)
        {
            int outlinerHeight = gApp.properties.docked ? (height - bottomHeight) / 2 : (height - bottomHeight);
            MoveWindow(gApp.outliner.hwnd, rightX, rightY, rightWidth, outlinerHeight, TRUE);
            rightY += outlinerHeight;
        }
        if (gApp.properties.docked)
        {
            MoveWindow(gApp.properties.hwnd, rightX, rightY, rightWidth, height - bottomHeight - rightY, TRUE);
        }
        if (gApp.timeline.docked)
        {
            MoveWindow(gApp.timeline.hwnd, leftWidth, height - bottomHeight, width - leftWidth - rightWidth, bottomHeight, TRUE);
        }

        MoveWindow(gApp.viewportWindow, viewportX, viewportY, viewportWidth, viewportHeight, TRUE);
        UpdateSwapChain();
    }

    void SetPanelDocked(Panel& panel, bool docked)
    {
        if (panel.docked == docked)
        {
            return;
        }
        panel.docked = docked;
        if (docked)
        {
            SetParent(panel.hwnd, gApp.mainWindow);
            SetWindowLongPtr(panel.hwnd, GWL_STYLE, WS_CHILD | WS_VISIBLE | WS_BORDER);
        }
        else
        {
            SetParent(panel.hwnd, nullptr);
            SetWindowLongPtr(panel.hwnd, GWL_STYLE, WS_OVERLAPPEDWINDOW | WS_VISIBLE);
            RECT rect{100, 100, 420, 480};
            AdjustWindowRect(&rect, WS_OVERLAPPEDWINDOW, FALSE);
            MoveWindow(panel.hwnd, rect.left, rect.top, rect.right - rect.left, rect.bottom - rect.top, TRUE);
        }
        ShowWindow(panel.hwnd, SW_SHOW);
        UpdateDockLayout(gApp.mainWindow);
    }

    void UpdateMenuChecks()
    {
        HMENU menu = GetMenu(gApp.mainWindow);
        HMENU windowMenu = GetSubMenu(menu, 3);
        CheckMenuItem(windowMenu, 4001, MF_BYCOMMAND | (gApp.toolbar.docked ? MF_CHECKED : MF_UNCHECKED));
        CheckMenuItem(windowMenu, 4002, MF_BYCOMMAND | (gApp.outliner.docked ? MF_CHECKED : MF_UNCHECKED));
        CheckMenuItem(windowMenu, 4003, MF_BYCOMMAND | (gApp.properties.docked ? MF_CHECKED : MF_UNCHECKED));
        CheckMenuItem(windowMenu, 4004, MF_BYCOMMAND | (gApp.timeline.docked ? MF_CHECKED : MF_UNCHECKED));
    }

    void CreateInspectorControls(HWND hwnd)
    {
        int y = 36;
        CreateWindowEx(0, WC_STATIC, L"Primitive", WS_CHILD | WS_VISIBLE,
            12, y, 120, 20, hwnd, nullptr, nullptr, nullptr);

        HWND combo = CreateWindowEx(0, WC_COMBOBOX, L"",
            WS_CHILD | WS_VISIBLE | CBS_DROPDOWNLIST,
            12, y + 22, 180, 300, hwnd, reinterpret_cast<HMENU>(5001), nullptr, nullptr);
        SendMessage(combo, CB_ADDSTRING, 0, reinterpret_cast<LPARAM>(L"Cube"));
        SendMessage(combo, CB_ADDSTRING, 0, reinterpret_cast<LPARAM>(L"Sphere"));
        SendMessage(combo, CB_ADDSTRING, 0, reinterpret_cast<LPARAM>(L"Grid"));
        SendMessage(combo, CB_SETCURSEL, 0, 0);

        y += 70;
        CreateWindowEx(0, WC_BUTTON, L"Wireframe", WS_CHILD | WS_VISIBLE | BS_AUTOCHECKBOX,
            12, y, 160, 24, hwnd, reinterpret_cast<HMENU>(5002), nullptr, nullptr);
        SendMessage(GetDlgItem(hwnd, 5002), BM_SETCHECK, gApp.wireframe ? BST_CHECKED : BST_UNCHECKED, 0);
        y += 28;
        CreateWindowEx(0, WC_BUTTON, L"Show Grid", WS_CHILD | WS_VISIBLE | BS_AUTOCHECKBOX,
            12, y, 160, 24, hwnd, reinterpret_cast<HMENU>(5003), nullptr, nullptr);
        SendMessage(GetDlgItem(hwnd, 5003), BM_SETCHECK, gApp.showGrid ? BST_CHECKED : BST_UNCHECKED, 0);

        y += 40;
        CreateWindowEx(0, WC_STATIC, L"Sphere Segments", WS_CHILD | WS_VISIBLE,
            12, y, 140, 18, hwnd, nullptr, nullptr, nullptr);
        HWND segSlider = CreateWindowEx(0, TRACKBAR_CLASS, L"",
            WS_CHILD | WS_VISIBLE | TBS_AUTOTICKS,
            12, y + 20, 220, 30, hwnd, reinterpret_cast<HMENU>(5004), nullptr, nullptr);
        SendMessage(segSlider, TBM_SETRANGE, TRUE, MAKELPARAM(8, 64));
        SendMessage(segSlider, TBM_SETPOS, TRUE, gApp.sphereSegments);

        y += 64;
        CreateWindowEx(0, WC_STATIC, L"Sphere Rings", WS_CHILD | WS_VISIBLE,
            12, y, 140, 18, hwnd, nullptr, nullptr, nullptr);
        HWND ringSlider = CreateWindowEx(0, TRACKBAR_CLASS, L"",
            WS_CHILD | WS_VISIBLE | TBS_AUTOTICKS,
            12, y + 20, 220, 30, hwnd, reinterpret_cast<HMENU>(5005), nullptr, nullptr);
        SendMessage(ringSlider, TBM_SETRANGE, TRUE, MAKELPARAM(6, 48));
        SendMessage(ringSlider, TBM_SETPOS, TRUE, gApp.sphereRings);
    }

    void DrawPanelBackground(HWND hwnd, HDC hdc)
    {
        RECT rect{};
        GetClientRect(hwnd, &rect);
        FillRect(hdc, &rect, gApp.panelBrush);

        RECT header = rect;
        header.bottom = 28;
        FillRect(hdc, &header, gApp.panelHeaderBrush);

        SetBkMode(hdc, TRANSPARENT);
        SetTextColor(hdc, gApp.panelTextColor);
        std::wstring title(64, L'\0');
        GetWindowText(hwnd, title.data(), static_cast<int>(title.size()));
        title.resize(wcslen(title.c_str()));
        DrawText(hdc, title.c_str(), -1, &header, DT_SINGLELINE | DT_VCENTER | DT_CENTER);
    }

    LRESULT CALLBACK PanelProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
    {
        switch (msg)
        {
        case WM_CREATE:
            if (hwnd == gApp.properties.hwnd)
            {
                CreateInspectorControls(hwnd);
            }
            return 0;
        case WM_PAINT:
        {
            PAINTSTRUCT ps{};
            HDC hdc = BeginPaint(hwnd, &ps);
            SelectObject(hdc, gApp.uiFont);
            DrawPanelBackground(hwnd, hdc);

            if (hwnd == gApp.toolbar.hwnd)
            {
                RECT rect{};
                GetClientRect(hwnd, &rect);
                rect.top = 40;
                DrawText(hdc, L"Move\nRotate\nScale\nInset\nBevel\nLoop Cut", -1, &rect, DT_TOP | DT_CENTER);
            }
            else if (hwnd == gApp.outliner.hwnd)
            {
                RECT rect{};
                GetClientRect(hwnd, &rect);
                rect.top = 40;
                DrawText(hdc, L"Scene\n  Camera\n  Light\n  Mesh", -1, &rect, DT_TOP | DT_LEFT | DT_NOPREFIX);
            }
            else if (hwnd == gApp.timeline.hwnd)
            {
                RECT rect{};
                GetClientRect(hwnd, &rect);
                rect.top = 40;
                DrawText(hdc, L"Timeline\nFrame 0001  |  Play  |  Stop  |  Key", -1, &rect, DT_TOP | DT_CENTER);
            }
            else if (hwnd == gApp.properties.hwnd)
            {
                RECT rect{};
                GetClientRect(hwnd, &rect);
                rect.top = 260;
                std::wstring text = L"Transform\n";
                text += L"Location: (" + std::to_wstring(gApp.meshPosition.x) + L", " + std::to_wstring(gApp.meshPosition.y) + L", " + std::to_wstring(gApp.meshPosition.z) + L")\n";
                text += L"Rotation: (" + std::to_wstring(gApp.meshRotation.x) + L", " + std::to_wstring(gApp.meshRotation.y) + L", " + std::to_wstring(gApp.meshRotation.z) + L")\n";
                text += L"Scale: (" + std::to_wstring(gApp.meshScale.x) + L", " + std::to_wstring(gApp.meshScale.y) + L", " + std::to_wstring(gApp.meshScale.z) + L")";
                DrawText(hdc, text.c_str(), -1, &rect, DT_LEFT | DT_TOP | DT_NOPREFIX);
            }

            EndPaint(hwnd, &ps);
            return 0;
        }
        case WM_CTLCOLORSTATIC:
        case WM_CTLCOLORBTN:
        {
            HDC hdc = reinterpret_cast<HDC>(wParam);
            SetTextColor(hdc, gApp.panelTextColor);
            SetBkColor(hdc, RGB(30, 30, 34));
            return reinterpret_cast<LRESULT>(gApp.panelBrush);
        }
        case WM_HSCROLL:
        {
            if (hwnd == gApp.properties.hwnd)
            {
                int controlId = GetDlgCtrlID(reinterpret_cast<HWND>(lParam));
                if (controlId == 5004)
                {
                    gApp.sphereSegments = static_cast<int>(SendMessage(reinterpret_cast<HWND>(lParam), TBM_GETPOS, 0, 0));
                    if (gApp.primitive == PrimitiveType::Sphere)
                    {
                        CreateDefaultMesh();
                    }
                }
                else if (controlId == 5005)
                {
                    gApp.sphereRings = static_cast<int>(SendMessage(reinterpret_cast<HWND>(lParam), TBM_GETPOS, 0, 0));
                    if (gApp.primitive == PrimitiveType::Sphere)
                    {
                        CreateDefaultMesh();
                    }
                }
            }
            return 0;
        }
        case WM_COMMAND:
        {
            if (hwnd == gApp.properties.hwnd)
            {
                int controlId = LOWORD(wParam);
                if (controlId == 5001 && HIWORD(wParam) == CBN_SELCHANGE)
                {
                    int index = static_cast<int>(SendMessage(reinterpret_cast<HWND>(lParam), CB_GETCURSEL, 0, 0));
                    gApp.primitive = static_cast<PrimitiveType>(index);
                    CreateDefaultMesh();
                    InvalidateRect(hwnd, nullptr, TRUE);
                }
                else if (controlId == 5002)
                {
                    gApp.wireframe = (SendMessage(reinterpret_cast<HWND>(lParam), BM_GETCHECK, 0, 0) == BST_CHECKED);
                }
                else if (controlId == 5003)
                {
                    gApp.showGrid = (SendMessage(reinterpret_cast<HWND>(lParam), BM_GETCHECK, 0, 0) == BST_CHECKED);
                }
            }
            return 0;
        }
        }
        return DefWindowProc(hwnd, msg, wParam, lParam);
    }

    LRESULT CALLBACK ViewportProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
    {
        switch (msg)
        {
        case WM_LBUTTONDOWN:
            gApp.orbiting = true;
            gApp.lastMouse = {GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam)};
            SetCapture(hwnd);
            return 0;
        case WM_MBUTTONDOWN:
            gApp.panning = true;
            gApp.lastMouse = {GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam)};
            SetCapture(hwnd);
            return 0;
        case WM_LBUTTONUP:
            gApp.orbiting = false;
            ReleaseCapture();
            return 0;
        case WM_MBUTTONUP:
            gApp.panning = false;
            ReleaseCapture();
            return 0;
        case WM_MOUSEMOVE:
        {
            POINT current{GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam)};
            float dx = static_cast<float>(current.x - gApp.lastMouse.x);
            float dy = static_cast<float>(current.y - gApp.lastMouse.y);
            if (gApp.orbiting)
            {
                gApp.camera.yaw += dx * 0.005f;
                gApp.camera.pitch += dy * 0.005f;
                gApp.camera.pitch = std::clamp(gApp.camera.pitch, -1.4f, 1.4f);
            }
            else if (gApp.panning)
            {
                gApp.camera.target.x -= dx * 0.01f;
                gApp.camera.target.y += dy * 0.01f;
            }
            gApp.lastMouse = current;
            return 0;
        }
        case WM_MOUSEWHEEL:
        {
            int delta = GET_WHEEL_DELTA_WPARAM(wParam);
            gApp.camera.distance = std::clamp(gApp.camera.distance - delta * 0.002f, 2.0f, 30.0f);
            return 0;
        }
        case WM_SIZE:
            UpdateSwapChain();
            return 0;
        }
        return DefWindowProc(hwnd, msg, wParam, lParam);
    }

    LRESULT CALLBACK MainProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
    {
        switch (msg)
        {
        case WM_CREATE:
            gApp.mainWindow = hwnd;
            return 0;
        case WM_COMMAND:
        {
            int id = LOWORD(wParam);
            switch (id)
            {
            case 4001:
                SetPanelDocked(gApp.toolbar, !gApp.toolbar.docked);
                UpdateMenuChecks();
                break;
            case 4002:
                SetPanelDocked(gApp.outliner, !gApp.outliner.docked);
                UpdateMenuChecks();
                break;
            case 4003:
                SetPanelDocked(gApp.properties, !gApp.properties.docked);
                UpdateMenuChecks();
                break;
            case 4004:
                SetPanelDocked(gApp.timeline, !gApp.timeline.docked);
                UpdateMenuChecks();
                break;
            }
            return 0;
        }
        case WM_SIZE:
            UpdateDockLayout(hwnd);
            return 0;
        case WM_TIMER:
            if (wParam == kTimerId)
            {
                RenderScene();
            }
            return 0;
        case WM_DESTROY:
            KillTimer(hwnd, kTimerId);
            DestroyDeviceResources();
            PostQuitMessage(0);
            return 0;
        case WM_CTLCOLORSTATIC:
        case WM_CTLCOLORBTN:
        {
            HDC hdc = reinterpret_cast<HDC>(wParam);
            SetTextColor(hdc, gApp.panelTextColor);
            SetBkColor(hdc, RGB(30, 30, 34));
            return reinterpret_cast<LRESULT>(gApp.panelBrush);
        }
        }
        return DefWindowProc(hwnd, msg, wParam, lParam);
    }

    HWND CreatePanelWindow(const wchar_t* title, HWND parent)
    {
        return CreateWindowEx(0, kPanelWindowClass, title,
            WS_CHILD | WS_VISIBLE | WS_BORDER,
            0, 0, 200, 200,
            parent, nullptr, GetModuleHandle(nullptr), nullptr);
    }

    void InitCommonControls()
    {
        INITCOMMONCONTROLSEX icc{};
        icc.dwSize = sizeof(icc);
        icc.dwICC = ICC_WIN95_CLASSES | ICC_BAR_CLASSES;
        InitCommonControlsEx(&icc);
    }

    void SetupMenu(HWND hwnd)
    {
        HMENU menu = CreateMenu();
        HMENU fileMenu = CreateMenu();
        HMENU editMenu = CreateMenu();
        HMENU viewMenu = CreateMenu();
        HMENU windowMenu = CreateMenu();
        HMENU helpMenu = CreateMenu();

        AppendMenu(fileMenu, MF_STRING, 1001, L"New Scene\tCtrl+N");
        AppendMenu(fileMenu, MF_STRING, 1002, L"Open...\tCtrl+O");
        AppendMenu(fileMenu, MF_STRING, 1003, L"Save\tCtrl+S");
        AppendMenu(fileMenu, MF_SEPARATOR, 0, nullptr);
        AppendMenu(fileMenu, MF_STRING, 1004, L"Exit");

        AppendMenu(editMenu, MF_STRING, 1101, L"Undo\tCtrl+Z");
        AppendMenu(editMenu, MF_STRING, 1102, L"Redo\tCtrl+Shift+Z");
        AppendMenu(editMenu, MF_STRING, 1103, L"Duplicate\tShift+D");

        AppendMenu(viewMenu, MF_STRING, 1201, L"Front View\tNumpad 1");
        AppendMenu(viewMenu, MF_STRING, 1202, L"Top View\tNumpad 7");
        AppendMenu(viewMenu, MF_STRING, 1203, L"Camera View\tNumpad 0");

        AppendMenu(windowMenu, MF_STRING, 4001, L"Dock Tool Shelf");
        AppendMenu(windowMenu, MF_STRING, 4002, L"Dock Outliner");
        AppendMenu(windowMenu, MF_STRING, 4003, L"Dock Properties");
        AppendMenu(windowMenu, MF_STRING, 4004, L"Dock Timeline");

        AppendMenu(helpMenu, MF_STRING, 1301, L"Controls");
        AppendMenu(helpMenu, MF_STRING, 1302, L"About");

        AppendMenu(menu, MF_POPUP, reinterpret_cast<UINT_PTR>(fileMenu), L"File");
        AppendMenu(menu, MF_POPUP, reinterpret_cast<UINT_PTR>(editMenu), L"Edit");
        AppendMenu(menu, MF_POPUP, reinterpret_cast<UINT_PTR>(viewMenu), L"View");
        AppendMenu(menu, MF_POPUP, reinterpret_cast<UINT_PTR>(windowMenu), L"Window");
        AppendMenu(menu, MF_POPUP, reinterpret_cast<UINT_PTR>(helpMenu), L"Help");

        SetMenu(hwnd, menu);
        UpdateMenuChecks();
    }

    void InitUITheme()
    {
        gApp.panelBrush = CreateSolidBrush(RGB(30, 30, 34));
        gApp.panelHeaderBrush = CreateSolidBrush(RGB(45, 45, 50));
        gApp.uiFont = CreateFont(16, 0, 0, 0, FW_MEDIUM, FALSE, FALSE, FALSE,
            DEFAULT_CHARSET, OUT_OUTLINE_PRECIS, CLIP_DEFAULT_PRECIS, CLEARTYPE_QUALITY,
            VARIABLE_PITCH, L"Segoe UI");
    }

    void DestroyUITheme()
    {
        if (gApp.panelBrush)
        {
            DeleteObject(gApp.panelBrush);
            gApp.panelBrush = nullptr;
        }
        if (gApp.panelHeaderBrush)
        {
            DeleteObject(gApp.panelHeaderBrush);
            gApp.panelHeaderBrush = nullptr;
        }
        if (gApp.uiFont)
        {
            DeleteObject(gApp.uiFont);
            gApp.uiFont = nullptr;
        }
    }
}

int WINAPI wWinMain(HINSTANCE instance, HINSTANCE, PWSTR, int)
{
    InitCommonControls();
    InitUITheme();

    WNDCLASSEX wc{};
    wc.cbSize = sizeof(wc);
    wc.style = CS_HREDRAW | CS_VREDRAW;
    wc.lpfnWndProc = MainProc;
    wc.hInstance = instance;
    wc.hCursor = LoadCursor(nullptr, IDC_ARROW);
    wc.hbrBackground = reinterpret_cast<HBRUSH>(COLOR_WINDOW + 1);
    wc.lpszClassName = kMainWindowClass;
    RegisterClassEx(&wc);

    WNDCLASSEX panelClass{};
    panelClass.cbSize = sizeof(panelClass);
    panelClass.style = CS_HREDRAW | CS_VREDRAW;
    panelClass.lpfnWndProc = PanelProc;
    panelClass.hInstance = instance;
    panelClass.hCursor = LoadCursor(nullptr, IDC_ARROW);
    panelClass.hbrBackground = reinterpret_cast<HBRUSH>(COLOR_WINDOW + 1);
    panelClass.lpszClassName = kPanelWindowClass;
    RegisterClassEx(&panelClass);

    WNDCLASSEX viewportClass{};
    viewportClass.cbSize = sizeof(viewportClass);
    viewportClass.style = CS_HREDRAW | CS_VREDRAW;
    viewportClass.lpfnWndProc = ViewportProc;
    viewportClass.hInstance = instance;
    viewportClass.hCursor = LoadCursor(nullptr, IDC_ARROW);
    viewportClass.hbrBackground = reinterpret_cast<HBRUSH>(COLOR_WINDOW + 1);
    viewportClass.lpszClassName = kViewportWindowClass;
    RegisterClassEx(&viewportClass);

    HWND hwnd = CreateWindowEx(0, kMainWindowClass, L"Broken Arrow MeshEdit",
        WS_OVERLAPPEDWINDOW | WS_VISIBLE,
        CW_USEDEFAULT, CW_USEDEFAULT, 1400, 900,
        nullptr, nullptr, instance, nullptr);

    gApp.toolbar.hwnd = CreatePanelWindow(L"Tool Shelf", hwnd);
    gApp.outliner.hwnd = CreatePanelWindow(L"Outliner", hwnd);
    gApp.properties.hwnd = CreatePanelWindow(L"Properties", hwnd);
    gApp.timeline.hwnd = CreatePanelWindow(L"Timeline", hwnd);

    gApp.viewportWindow = CreateWindowEx(0, kViewportWindowClass, L"Viewport",
        WS_CHILD | WS_VISIBLE,
        0, 0, 800, 600, hwnd, nullptr, instance, nullptr);

    SetupMenu(hwnd);
    UpdateDockLayout(hwnd);

    CreateDeviceResources(gApp.viewportWindow);
    CreateRenderTargets();
    CreateShaders();
    UpdateGridResources();
    CreateDefaultMesh();

    SetTimer(hwnd, kTimerId, 16, nullptr);

    MSG msg{};
    while (msg.message != WM_QUIT)
    {
        while (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }

    DestroyUITheme();
    return 0;
}
