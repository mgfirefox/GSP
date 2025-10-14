#pragma once
#include <d3d11.h>
#include <DirectXMath.h>
#include <d3dcompiler.h>

#include <wrl/client.h>
#include <memory>

#include <sstream>

#include <vector>
#include <array>

#include <string>

#include "Window.h"

#include "IntUtility.h"

#include "MemoryUtility.h"
#include "Direct3dUtility.h"

class Direct3d
{
    bool initialized;
    bool released;

    std::shared_ptr<Window> window;

    bool vsyncEnabled;

    AdapterData adapterData;

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
    Direct3d(std::shared_ptr<Window> window);
    ~Direct3d();

private:
    bool isInitialized();
    void setInitialized();

    bool isReleased();
    void setReleased();

public:
    AdapterData getAdapterData();

    Microsoft::WRL::ComPtr<ID3D11Device> getDevice();
    Microsoft::WRL::ComPtr<ID3D11DeviceContext> getDeviceContext();

    bool initialize(bool vsyncEnabled);
    void release();

    bool createVertexShader(Microsoft::WRL::ComPtr<ID3D11VertexShader>& vertexShader,
                            std::wstring vertexShaderFilename,
                            Microsoft::WRL::ComPtr<ID3D10Blob>& vertexShaderBuffer);
    bool createPixelShader(Microsoft::WRL::ComPtr<ID3D11PixelShader>& pixelShader,
                           std::wstring pixelShaderFilename);
    void showShaderErrorMessage(Microsoft::WRL::ComPtr<ID3D10Blob> errorMessage,
                                std::wstring shaderFilename);
    bool setVertexShader(Microsoft::WRL::ComPtr<ID3D11VertexShader> vertexShader);
    bool setPixelShader(Microsoft::WRL::ComPtr<ID3D11PixelShader> pixelShader);

    bool createInputLayout(Microsoft::WRL::ComPtr<ID3D11InputLayout>& inputLayout,
                           const D3D11_INPUT_ELEMENT_DESC* inputElementDescs,
                           uint32 inputElementDescCount,
                           Microsoft::WRL::ComPtr<ID3D10Blob> vertexShaderBuffer);
    bool setInputLayoutToInputAssembler(Microsoft::WRL::ComPtr<ID3D11InputLayout> inputLayout);

    bool createConstantBuffer(Microsoft::WRL::ComPtr<ID3D11Buffer>& constantBuffer,
                              D3D11_BUFFER_DESC constantBufferDesc, uint32&
                              actualConstantBufferSize);
    bool setConstantBufferToVertexShader(Microsoft::WRL::ComPtr<ID3D11Buffer> constantBuffer,
                                         uint32 slotIndex);
    bool setConstantBufferToPixelShader(Microsoft::WRL::ComPtr<ID3D11Buffer> constantBuffer,
                                        uint32 slotIndex);
    bool setConstantBufferData(Microsoft::WRL::ComPtr<ID3D11Buffer> constantBuffer,
                               const void* constantBufferData, uint32
                               constantBufferSize);

    bool createSamplerState(Microsoft::WRL::ComPtr<ID3D11SamplerState>& samplerState,
                            D3D11_SAMPLER_DESC samplerStateDesc);
    bool setSamplerStateToPixelShader(Microsoft::WRL::ComPtr<ID3D11SamplerState> samplerState,
                                      uint32 slotIndex);

    bool createVertexBuffer(Microsoft::WRL::ComPtr<ID3D11Buffer>& vertexBuffer,
                            const void* vertexes, uint32 vertexStride, uint32 vertexCount);
    bool setVertexBufferToInputAssembler(Microsoft::WRL::ComPtr<ID3D11Buffer> vertexBuffer,
                                         uint32 vertexStride);

    bool createIndexBuffer(Microsoft::WRL::ComPtr<ID3D11Buffer>& indexBuffer, const uint32* indexes,
                           uint32
                           indexCount);
    bool setIndexBufferToInputAssembler(Microsoft::WRL::ComPtr<ID3D11Buffer> indexBuffer);

    bool canMipmapBeGenerated(DXGI_FORMAT format);
    bool createTexture2d(Microsoft::WRL::ComPtr<ID3D11Texture2D>& texture2d,
                         D3D11_TEXTURE2D_DESC texture2dDesc,
                         const D3D11_SUBRESOURCE_DATA* initialData);

    bool createShaderResourceView(
        Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>& shaderResourceView,
        D3D11_SHADER_RESOURCE_VIEW_DESC shaderResourceViewDesc,
        Microsoft::WRL::ComPtr<ID3D11Texture2D> texture2d, bool shouldGenerateMipmaps = false);
    bool setShaderResourceViewToPixelShader(
        Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> shaderResourceView, uint32 slotIndex);

    bool drawIndexed(uint32 indexCount);

    void setBackBufferRenderTarget();
    void resetViewport();

    void onFrameStarted(DirectX::XMFLOAT4 color);
    void onFrameFinished();

private:
    bool getAdapter(Microsoft::WRL::ComPtr<IDXGIAdapter>& adapter);
    bool initializeAdapterData(Microsoft::WRL::ComPtr<IDXGIAdapter> adapter);
    bool createDisplayModeDesc(
        Microsoft::WRL::ComPtr<IDXGIAdapter> adapter,
        DXGI_MODE_DESC& displayModeDesc);
    bool initializeSwapChain(DXGI_MODE_DESC displayModeDesc,
                             DXGI_SWAP_CHAIN_DESC& swapChainDesc);
    bool initializeRenderTargetView();
    bool initializeDepthStencilBuffer(
        DXGI_SWAP_CHAIN_DESC swapChainDesc);
    bool initializeDepthStencilState();
    bool initializeDepthStencilView();
    bool initializeRasterizerState();
    bool initializeViewport();
};
