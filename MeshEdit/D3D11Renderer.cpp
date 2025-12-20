#include "D3D11Renderer.h"

#include <array>
#include <cmath>
#include <cstring>
#include <d3dcompiler.h>

namespace {
    constexpr float kClearColor[4] = { 0.07f, 0.08f, 0.12f, 1.0f };

    std::array<float, 16> Multiply(const std::array<float, 16>& a, const std::array<float, 16>& b) {
        std::array<float, 16> out{};
        for (int row = 0; row < 4; ++row) {
            for (int col = 0; col < 4; ++col) {
                out[row * 4 + col] =
                    a[row * 4 + 0] * b[0 * 4 + col] +
                    a[row * 4 + 1] * b[1 * 4 + col] +
                    a[row * 4 + 2] * b[2 * 4 + col] +
                    a[row * 4 + 3] * b[3 * 4 + col];
            }
        }
        return out;
    }

    std::array<float, 16> Identity() {
        return { 1, 0, 0, 0,
                 0, 1, 0, 0,
                 0, 0, 1, 0,
                 0, 0, 0, 1 };
    }

    std::array<float, 16> Perspective(float fov, float aspect, float nearZ, float farZ) {
        const float f = 1.0f / std::tan(fov * 0.5f);
        std::array<float, 16> m{};
        m[0] = f / aspect;
        m[5] = f;
        m[10] = farZ / (farZ - nearZ);
        m[11] = 1.0f;
        m[14] = (-nearZ * farZ) / (farZ - nearZ);
        return m;
    }

    std::array<float, 16> Translation(float x, float y, float z) {
        auto m = Identity();
        m[12] = x;
        m[13] = y;
        m[14] = z;
        return m;
    }

    std::array<float, 16> Rotation(float yaw, float pitch) {
        const float cy = std::cos(yaw);
        const float sy = std::sin(yaw);
        const float cx = std::cos(pitch);
        const float sx = std::sin(pitch);

        std::array<float, 16> m{};
        m[0] = cy;
        m[1] = sx * sy;
        m[2] = -cx * sy;
        m[3] = 0.0f;
        m[4] = 0.0f;
        m[5] = cx;
        m[6] = sx;
        m[7] = 0.0f;
        m[8] = sy;
        m[9] = -sx * cy;
        m[10] = cx * cy;
        m[11] = 0.0f;
        m[12] = 0.0f;
        m[13] = 0.0f;
        m[14] = 0.0f;
        m[15] = 1.0f;
        return m;
    }
}

bool D3D11Renderer::initialize(HWND hwnd, int width, int height) {
    width_ = width;
    height_ = height;
    if (!createDeviceAndSwapChain(hwnd, width, height)) {
        return false;
    }
    if (!createRenderTarget(width, height)) {
        return false;
    }
    if (!createShaders()) {
        return false;
    }
    if (!createGeometry()) {
        return false;
    }
    return true;
}

bool D3D11Renderer::createDeviceAndSwapChain(HWND hwnd, int width, int height) {
    DXGI_SWAP_CHAIN_DESC desc{};
    desc.BufferDesc.Width = static_cast<UINT>(width);
    desc.BufferDesc.Height = static_cast<UINT>(height);
    desc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    desc.SampleDesc.Count = 1;
    desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    desc.BufferCount = 2;
    desc.OutputWindow = hwnd;
    desc.Windowed = TRUE;
    desc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;

    const UINT deviceFlags = D3D11_CREATE_DEVICE_BGRA_SUPPORT;
    D3D_FEATURE_LEVEL featureLevel = D3D_FEATURE_LEVEL_11_0;
    HRESULT hr = D3D11CreateDeviceAndSwapChain(
        nullptr,
        D3D_DRIVER_TYPE_HARDWARE,
        nullptr,
        deviceFlags,
        &featureLevel,
        1,
        D3D11_SDK_VERSION,
        &desc,
        swapChain_.GetAddressOf(),
        device_.GetAddressOf(),
        nullptr,
        context_.GetAddressOf());
    return SUCCEEDED(hr);
}

bool D3D11Renderer::createRenderTarget(int width, int height) {
    Microsoft::WRL::ComPtr<ID3D11Texture2D> backBuffer;
    if (FAILED(swapChain_->GetBuffer(0, IID_PPV_ARGS(&backBuffer)))) {
        return false;
    }
    if (FAILED(device_->CreateRenderTargetView(backBuffer.Get(), nullptr, renderTargetView_.GetAddressOf()))) {
        return false;
    }
    D3D11_VIEWPORT viewport{};
    viewport.Width = static_cast<float>(width);
    viewport.Height = static_cast<float>(height);
    viewport.MinDepth = 0.0f;
    viewport.MaxDepth = 1.0f;
    context_->RSSetViewports(1, &viewport);
    return true;
}

bool D3D11Renderer::createShaders() {
    const char* vsSource =
        "cbuffer Constants : register(b0) { float4x4 mvp; };"
        "struct VSInput { float3 pos : POSITION; float3 col : COLOR; };"
        "struct PSInput { float4 pos : SV_POSITION; float3 col : COLOR; };"
        "PSInput main(VSInput input) {"
        "  PSInput outp;"
        "  outp.pos = mul(float4(input.pos, 1.0f), mvp);"
        "  outp.col = input.col;"
        "  return outp;"
        "}";
    const char* psSource =
        "struct PSInput { float4 pos : SV_POSITION; float3 col : COLOR; };"
        "float4 main(PSInput input) : SV_Target { return float4(input.col, 1.0f); }";

    Microsoft::WRL::ComPtr<ID3DBlob> vsBlob;
    Microsoft::WRL::ComPtr<ID3DBlob> psBlob;
    Microsoft::WRL::ComPtr<ID3DBlob> errorBlob;

    if (FAILED(D3DCompile(vsSource, std::strlen(vsSource), nullptr, nullptr, nullptr, "main", "vs_5_0", 0, 0, &vsBlob, &errorBlob))) {
        return false;
    }
    if (FAILED(D3DCompile(psSource, std::strlen(psSource), nullptr, nullptr, nullptr, "main", "ps_5_0", 0, 0, &psBlob, &errorBlob))) {
        return false;
    }
    if (FAILED(device_->CreateVertexShader(vsBlob->GetBufferPointer(), vsBlob->GetBufferSize(), nullptr, vertexShader_.GetAddressOf()))) {
        return false;
    }
    if (FAILED(device_->CreatePixelShader(psBlob->GetBufferPointer(), psBlob->GetBufferSize(), nullptr, pixelShader_.GetAddressOf()))) {
        return false;
    }
    const D3D11_INPUT_ELEMENT_DESC layout[] = {
        { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "COLOR", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 }
    };
    if (FAILED(device_->CreateInputLayout(layout, 2, vsBlob->GetBufferPointer(), vsBlob->GetBufferSize(), inputLayout_.GetAddressOf()))) {
        return false;
    }
    return true;
}

bool D3D11Renderer::createGeometry() {
    const Vertex vertices[] = {
        { { -0.5f, -0.5f, 0.0f }, { 0.9f, 0.2f, 0.2f } },
        { {  0.0f,  0.5f, 0.0f }, { 0.2f, 0.9f, 0.2f } },
        { {  0.5f, -0.5f, 0.0f }, { 0.2f, 0.2f, 0.9f } }
    };
    D3D11_BUFFER_DESC vbDesc{};
    vbDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    vbDesc.ByteWidth = sizeof(vertices);
    vbDesc.Usage = D3D11_USAGE_IMMUTABLE;
    D3D11_SUBRESOURCE_DATA vbData{};
    vbData.pSysMem = vertices;
    if (FAILED(device_->CreateBuffer(&vbDesc, &vbData, vertexBuffer_.GetAddressOf()))) {
        return false;
    }

    D3D11_BUFFER_DESC cbDesc{};
    cbDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    cbDesc.ByteWidth = sizeof(Constants);
    cbDesc.Usage = D3D11_USAGE_DYNAMIC;
    cbDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    if (FAILED(device_->CreateBuffer(&cbDesc, nullptr, constantBuffer_.GetAddressOf()))) {
        return false;
    }
    return true;
}

void D3D11Renderer::resize(int width, int height) {
    if (!swapChain_) {
        return;
    }
    width_ = width;
    height_ = height;
    renderTargetView_.Reset();
    swapChain_->ResizeBuffers(0, static_cast<UINT>(width), static_cast<UINT>(height), DXGI_FORMAT_UNKNOWN, 0);
    createRenderTarget(width, height);
}

void D3D11Renderer::update(float deltaSeconds) {
    time_ += deltaSeconds;
    updateConstantBuffer();
}

void D3D11Renderer::render() {
    if (!context_ || !renderTargetView_) {
        return;
    }
    context_->OMSetRenderTargets(1, renderTargetView_.GetAddressOf(), nullptr);
    context_->ClearRenderTargetView(renderTargetView_.Get(), kClearColor);

    const UINT stride = sizeof(Vertex);
    const UINT offset = 0;
    context_->IASetVertexBuffers(0, 1, vertexBuffer_.GetAddressOf(), &stride, &offset);
    context_->IASetInputLayout(inputLayout_.Get());
    context_->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    context_->VSSetShader(vertexShader_.Get(), nullptr, 0);
    context_->PSSetShader(pixelShader_.Get(), nullptr, 0);
    context_->VSSetConstantBuffers(0, 1, constantBuffer_.GetAddressOf());
    context_->Draw(3, 0);

    swapChain_->Present(1, 0);
}

void D3D11Renderer::addRotation(float yawDelta, float pitchDelta) {
    yaw_ += yawDelta;
    pitch_ += pitchDelta;
}

void D3D11Renderer::updateConstantBuffer() {
    const float aspect = height_ > 0 ? static_cast<float>(width_) / static_cast<float>(height_) : 1.0f;
    const auto projection = Perspective(1.1f, aspect, 0.1f, 100.0f);
    const auto view = Translation(0.0f, 0.0f, 2.5f);
    const auto rotation = Rotation(yaw_ + time_ * 0.2f, pitch_);
    const auto vp = Multiply(rotation, view);
    const auto mvp = Multiply(vp, projection);

    D3D11_MAPPED_SUBRESOURCE mapped{};
    if (SUCCEEDED(context_->Map(constantBuffer_.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mapped))) {
        auto* data = reinterpret_cast<Constants*>(mapped.pData);
        std::copy(mvp.begin(), mvp.end(), data->mvp);
        context_->Unmap(constantBuffer_.Get(), 0);
    }
}

bool D3D11Renderer::isInitialized() const {
    return device_ != nullptr;
}
