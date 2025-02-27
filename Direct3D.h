#pragma once
#include <d3d11.h>
#include <DirectXMath.h>

#include <wrl/client.h>
#include <memory>

class Direct3D {
    bool initialized;
    bool released;

    bool vsyncEnabled;

    int videoCardMemory;
    char videoCardDescription[128];

    Microsoft::WRL::ComPtr<IDXGISwapChain> swapChain;
    Microsoft::WRL::ComPtr<ID3D11Device> device;
    Microsoft::WRL::ComPtr<ID3D11DeviceContext> deviceContext;
    Microsoft::WRL::ComPtr<ID3D11RenderTargetView> renderTargetView;

    Microsoft::WRL::ComPtr<ID3D11Texture2D> depthStencilBuffer;
    Microsoft::WRL::ComPtr<ID3D11DepthStencilState> depthStencilState;
    Microsoft::WRL::ComPtr<ID3D11DepthStencilView> depthStencilView;
    Microsoft::WRL::ComPtr<ID3D11RasterizerState> rasterizerState;

    D3D11_VIEWPORT viewport;

public:
    Direct3D();
    ~Direct3D();

private:
    bool isInitialized();
    void setInitialized();

    bool isReleased();
    void setReleased();

public:
    void getVideoCardData(char* name, int& memory);

    Microsoft::WRL::ComPtr<ID3D11Device> getDevice();
    Microsoft::WRL::ComPtr<ID3D11DeviceContext> getDeviceContext();

    bool initialize(int screenWidth, int screenHeight, HWND windowHandle, bool vsyncEnabled, bool fullscreenEnabled);
    void release();

    void beginScene(DirectX::XMFLOAT4 color);
    void endScene();

    void setBackBufferRenderTarget();
    void resetViewport();
};
