#pragma once

#include <d3d11.h>
#include <wrl/client.h>

class D3D11Renderer {
public:
    bool initialize(HWND hwnd, int width, int height);
    void resize(int width, int height);
    void update(float deltaSeconds);
    void render();
    void addRotation(float yawDelta, float pitchDelta);
    bool isInitialized() const;

private:
    bool createDeviceAndSwapChain(HWND hwnd, int width, int height);
    bool createRenderTarget(int width, int height);
    bool createShaders();
    bool createGeometry();
    void updateConstantBuffer();

    struct Vertex {
        float position[3];
        float color[3];
    };

    struct Constants {
        float mvp[16];
    };

    Microsoft::WRL::ComPtr<ID3D11Device> device_;
    Microsoft::WRL::ComPtr<ID3D11DeviceContext> context_;
    Microsoft::WRL::ComPtr<IDXGISwapChain> swapChain_;
    Microsoft::WRL::ComPtr<ID3D11RenderTargetView> renderTargetView_;
    Microsoft::WRL::ComPtr<ID3D11Buffer> vertexBuffer_;
    Microsoft::WRL::ComPtr<ID3D11Buffer> constantBuffer_;
    Microsoft::WRL::ComPtr<ID3D11VertexShader> vertexShader_;
    Microsoft::WRL::ComPtr<ID3D11PixelShader> pixelShader_;
    Microsoft::WRL::ComPtr<ID3D11InputLayout> inputLayout_;

    float yaw_ = 0.0f;
    float pitch_ = 0.0f;
    float time_ = 0.0f;
    int width_ = 0;
    int height_ = 0;
};
