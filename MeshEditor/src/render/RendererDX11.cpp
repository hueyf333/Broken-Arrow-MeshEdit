// File: MeshEditor/src/render/RendererDX11.cpp
#include "RendererDX11.h"

#include <vector>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/euler_angles.hpp>

#include "engine/FileSystem.h"
#include "engine/Math.h"
#include "render/RenderMesh.h"

namespace mesh
{
struct SceneConstants
{
    glm::mat4 view;
    glm::mat4 projection;
    glm::mat4 model;
    glm::vec4 color;
    glm::vec4 selectionColor;
    int useTexture;
    glm::vec3 padding{};
};

bool RendererDX11::Initialize(HWND hwnd, int width, int height)
{
    hwnd_ = hwnd;
    width_ = width;
    height_ = height;

    DXGI_SWAP_CHAIN_DESC desc{};
    desc.BufferCount = 1;
    desc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    desc.OutputWindow = hwnd;
    desc.SampleDesc.Count = 1;
    desc.Windowed = TRUE;

    UINT flags = 0;
#if defined(_DEBUG)
    flags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

    D3D_FEATURE_LEVEL featureLevel;
    if (FAILED(D3D11CreateDeviceAndSwapChain(nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr, flags,
                                             nullptr, 0, D3D11_SDK_VERSION, &desc,
                                             swapChain_.GetAddressOf(), device_.GetAddressOf(),
                                             &featureLevel, context_.GetAddressOf())))
    {
        return false;
    }

    CreateRenderTargets(width, height);
    CreateStates();

    auto meshShaderPath = FileSystem::GetShaderPath("BasicMesh.hlsl");
    auto gridShaderPath = FileSystem::GetShaderPath("Grid.hlsl");
    meshShader_.Load(device_.Get(), meshShaderPath.wstring(), "VSMain", "PSMain");
    std::vector<D3D11_INPUT_ELEMENT_DESC> gridLayout =
    {
        {"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
        {"COLOR", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0}
    };
    gridShader_.LoadWithLayout(device_.Get(), gridShaderPath.wstring(), "VSMain", "PSMain", gridLayout);
    grid_.Initialize(device_.Get());

    D3D11_BUFFER_DESC cbDesc{};
    cbDesc.ByteWidth = sizeof(SceneConstants);
    cbDesc.Usage = D3D11_USAGE_DEFAULT;
    cbDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    device_->CreateBuffer(&cbDesc, nullptr, constantBuffer_.GetAddressOf());

    camera_.SetViewport(static_cast<float>(width), static_cast<float>(height));

    return true;
}

void RendererDX11::CreateRenderTargets(int width, int height)
{
    rtv_.Reset();
    dsv_.Reset();
    depthBuffer_.Reset();

    Microsoft::WRL::ComPtr<ID3D11Texture2D> backBuffer;
    swapChain_->GetBuffer(0, IID_PPV_ARGS(backBuffer.GetAddressOf()));
    device_->CreateRenderTargetView(backBuffer.Get(), nullptr, rtv_.GetAddressOf());

    D3D11_TEXTURE2D_DESC depthDesc{};
    depthDesc.Width = width;
    depthDesc.Height = height;
    depthDesc.MipLevels = 1;
    depthDesc.ArraySize = 1;
    depthDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
    depthDesc.SampleDesc.Count = 1;
    depthDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
    device_->CreateTexture2D(&depthDesc, nullptr, depthBuffer_.GetAddressOf());
    device_->CreateDepthStencilView(depthBuffer_.Get(), nullptr, dsv_.GetAddressOf());
}

void RendererDX11::CreateStates()
{
    D3D11_RASTERIZER_DESC solid{};
    solid.FillMode = D3D11_FILL_SOLID;
    solid.CullMode = D3D11_CULL_BACK;
    solid.DepthClipEnable = TRUE;
    device_->CreateRasterizerState(&solid, rasterSolid_.GetAddressOf());

    D3D11_RASTERIZER_DESC wire = solid;
    wire.FillMode = D3D11_FILL_WIREFRAME;
    device_->CreateRasterizerState(&wire, rasterWire_.GetAddressOf());

    D3D11_SAMPLER_DESC samp{};
    samp.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
    samp.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
    samp.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
    samp.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
    device_->CreateSamplerState(&samp, sampler_.GetAddressOf());
}

void RendererDX11::Resize(int width, int height)
{
    if (!swapChain_)
    {
        return;
    }
    width_ = width;
    height_ = height;
    context_->OMSetRenderTargets(0, nullptr, nullptr);
    rtv_.Reset();
    dsv_.Reset();
    depthBuffer_.Reset();
    swapChain_->ResizeBuffers(0, width, height, DXGI_FORMAT_UNKNOWN, 0);
    CreateRenderTargets(width, height);
    camera_.SetViewport(static_cast<float>(width), static_cast<float>(height));
}

TextureDX11* RendererDX11::GetTexture(const std::string& path)
{
    if (path.empty())
    {
        return nullptr;
    }
    auto found = textures_.find(path);
    if (found != textures_.end())
    {
        return found->second.get();
    }
    auto texture = std::make_unique<TextureDX11>();
    std::wstring wide(path.begin(), path.end());
    if (!texture->Load(device_.Get(), wide))
    {
        return nullptr;
    }
    auto* result = texture.get();
    textures_.emplace(path, std::move(texture));
    return result;
}

void RendererDX11::Render(const Scene& scene, const Selection& selection)
{
    if (!context_ || !rtv_ || !dsv_)
    {
        return;
    }

    float clearColor[4] = {0.08f, 0.08f, 0.1f, 1.0f};
    context_->OMSetRenderTargets(1, rtv_.GetAddressOf(), dsv_.Get());
    context_->ClearRenderTargetView(rtv_.Get(), clearColor);
    context_->ClearDepthStencilView(dsv_.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

    D3D11_VIEWPORT viewport{};
    viewport.Width = static_cast<float>(width_);
    viewport.Height = static_cast<float>(height_);
    viewport.MinDepth = 0.0f;
    viewport.MaxDepth = 1.0f;
    context_->RSSetViewports(1, &viewport);

    context_->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    context_->RSSetState(wireframe_ ? rasterWire_.Get() : rasterSolid_.Get());

    SceneConstants constants{};
    constants.view = camera_.ViewMatrix();
    constants.projection = camera_.ProjectionMatrix();
    constants.selectionColor = glm::vec4(1.0f, 0.7f, 0.2f, 1.0f);

    meshShader_.Bind(context_.Get());

    for (const auto& entityPtr : scene.Entities())
    {
        const auto& entity = *entityPtr;
        if (!entity.Mesh().mesh)
        {
            continue;
        }
        const auto& transform = entity.Transform();
        glm::mat4 model = glm::translate(glm::mat4(1.0f), transform.position)
            * glm::yawPitchRoll(transform.rotation.y, transform.rotation.x, transform.rotation.z)
            * glm::scale(glm::mat4(1.0f), transform.scale);

        constants.model = model;
        constants.color = entity.Material().baseColor;
        if (selection.HasSelection() && selection.Selected().value() == entity.Id())
        {
            constants.color = constants.selectionColor;
        }
        constants.useTexture = 0;

        ID3D11ShaderResourceView* srv = nullptr;
        if (shaded_)
        {
            if (auto* texture = GetTexture(entity.Material().baseColorTexture))
            {
                srv = texture->SRV();
                constants.useTexture = srv ? 1 : 0;
            }
        }

        context_->UpdateSubresource(constantBuffer_.Get(), 0, nullptr, &constants, 0, 0);
        context_->VSSetConstantBuffers(0, 1, constantBuffer_.GetAddressOf());
        context_->PSSetConstantBuffers(0, 1, constantBuffer_.GetAddressOf());
        if (srv)
        {
            context_->PSSetShaderResources(0, 1, &srv);
            context_->PSSetSamplers(0, 1, sampler_.GetAddressOf());
        }

        entity.Mesh().mesh->Draw(context_.Get());
    }

    // Draw grid
    context_->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_LINELIST);
    gridShader_.Bind(context_.Get());
    SceneConstants gridConstants{};
    gridConstants.view = camera_.ViewMatrix();
    gridConstants.projection = camera_.ProjectionMatrix();
    gridConstants.model = glm::mat4(1.0f);
    gridConstants.color = glm::vec4(1.0f);
    context_->UpdateSubresource(constantBuffer_.Get(), 0, nullptr, &gridConstants, 0, 0);
    context_->VSSetConstantBuffers(0, 1, constantBuffer_.GetAddressOf());
    grid_.Draw(context_.Get());

    swapChain_->Present(1, 0);
}
}
