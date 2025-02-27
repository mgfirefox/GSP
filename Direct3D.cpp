#include "Direct3D.h"

Direct3D::Direct3D() : videoCardDescription{}, swapChain(), device(), deviceContext(), renderTargetView(), depthStencilBuffer(), depthStencilState(), depthStencilView(), rasterizerState(), viewport{} {
    initialized = false;
    released = false;
    
    vsyncEnabled = false;
    videoCardMemory = 0;
}

Direct3D::~Direct3D() {
    release();
}

bool Direct3D::isInitialized() {
    return initialized;
}

void Direct3D::setInitialized() {
    initialized = true;
    released = false;
}

bool Direct3D::isReleased() {
    return released;
}

void Direct3D::setReleased() {
    initialized = false;
    released = true;
}

bool Direct3D::initialize(int screenWidth, int screenHeight, HWND windowHandle, bool vsyncEnabled, bool fullscreenEnabled) {
    if (isInitialized()) {
        release();
    }

    this->vsyncEnabled = vsyncEnabled;
    
    Microsoft::WRL::ComPtr<IDXGIFactory> factory;
    HRESULT result = CreateDXGIFactory(__uuidof(IDXGIFactory), (void**)factory.GetAddressOf());
    if (FAILED(result)) {
        return false;
    }

    Microsoft::WRL::ComPtr<IDXGIAdapter> adapter;
    result = factory->EnumAdapters(0, adapter.GetAddressOf());
    if (FAILED(result)) {
        return false;
    }

    factory.Reset();

    Microsoft::WRL::ComPtr<IDXGIOutput> adapterOutput;
    result = adapter->EnumOutputs(0, adapterOutput.GetAddressOf());
    if (FAILED(result)) {
        return false;
    }

    DXGI_FORMAT format = DXGI_FORMAT_R8G8B8A8_UNORM;
    unsigned int displayModeListSize = 0;
    result = adapterOutput->GetDisplayModeList(format, DXGI_ENUM_MODES_INTERLACED, &displayModeListSize, NULL);
    if (FAILED(result) || displayModeListSize == 0) {
        return false;
    }
    std::unique_ptr<DXGI_MODE_DESC[]> displayModeList = std::make_unique<DXGI_MODE_DESC[]>(displayModeListSize);
    result = adapterOutput->GetDisplayModeList(format, DXGI_ENUM_MODES_INTERLACED, &displayModeListSize, displayModeList.get());
    if (FAILED(result)) {
        return false;
    }

    adapterOutput.Reset();

    DXGI_MODE_DESC displayModeDesc = {};
    displayModeDesc.Width = screenWidth;
    displayModeDesc.Height = screenHeight;

    DXGI_RATIONAL& refreshRate = displayModeDesc.RefreshRate;
    refreshRate.Numerator = 0;
    refreshRate.Denominator = 1;

    displayModeDesc.Format = format;
    displayModeDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
    displayModeDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;

    if (this->vsyncEnabled) {
        for (unsigned int i = 0; i < displayModeListSize; i++) {
            if (displayModeList[i].Width == screenWidth
                && displayModeList[i].Height == screenHeight) {
                displayModeDesc.RefreshRate = displayModeList[i].RefreshRate;
                break;
            }
        }
    }
    displayModeList.reset();

    DXGI_ADAPTER_DESC adapterDesc;
    result = adapter->GetDesc(&adapterDesc);
    if (FAILED(result)) {
        return false;
    }

    unsigned long long stringLength = 0;
    int error = wcstombs_s(&stringLength, videoCardDescription, sizeof(videoCardDescription), adapterDesc.Description, sizeof(adapterDesc.Description));
    if (error != 0) {
        return false;
    }
    videoCardMemory = (int)(adapterDesc.DedicatedVideoMemory / 1024 / 1024);

    adapter.Reset();

    DXGI_SWAP_CHAIN_DESC swapChainDesc = {};
    swapChainDesc.BufferDesc = displayModeDesc;

    DXGI_SAMPLE_DESC& sampleDesc = swapChainDesc.SampleDesc;
    sampleDesc.Count = 1;
    sampleDesc.Quality = 0;

    swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    swapChainDesc.BufferCount = 1;
    swapChainDesc.OutputWindow = windowHandle;
    swapChainDesc.Windowed = !fullscreenEnabled;
    swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
    swapChainDesc.Flags = 0;

    D3D_FEATURE_LEVEL featureLevel = D3D_FEATURE_LEVEL_11_0;
    result = D3D11CreateDeviceAndSwapChain(NULL, D3D_DRIVER_TYPE_HARDWARE, NULL, 0, &featureLevel, 1, D3D11_SDK_VERSION, &swapChainDesc, swapChain.GetAddressOf(), device.GetAddressOf(), NULL, deviceContext.GetAddressOf());
    if (FAILED(result)) {
        return false;
    }

    Microsoft::WRL::ComPtr<ID3D11Texture2D> backBuffer;
    result = swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)backBuffer.GetAddressOf());
    if (FAILED(result)) {
        return false;
    }
    result = device->CreateRenderTargetView(backBuffer.Get(), NULL, renderTargetView.GetAddressOf());
    if (FAILED(result)) {
        return false;
    }
    backBuffer.Reset();

    D3D11_TEXTURE2D_DESC depthBufferDesc = {};
    depthBufferDesc.Width = screenWidth;
    depthBufferDesc.Height = screenHeight;
    depthBufferDesc.MipLevels = 1;
    depthBufferDesc.ArraySize = 1;
    depthBufferDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
    depthBufferDesc.SampleDesc = swapChainDesc.SampleDesc;
    depthBufferDesc.Usage = D3D11_USAGE_DEFAULT;
    depthBufferDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
    depthBufferDesc.CPUAccessFlags = 0;
    depthBufferDesc.MiscFlags = 0;

    result = device->CreateTexture2D(&depthBufferDesc, NULL, depthStencilBuffer.GetAddressOf());
    if (FAILED(result)) {
        return false;
    }

    D3D11_DEPTH_STENCIL_DESC depthStencilDesc = {};
    depthStencilDesc.DepthEnable = true;
    depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
    depthStencilDesc.DepthFunc = D3D11_COMPARISON_LESS;
    depthStencilDesc.StencilEnable = true;
    depthStencilDesc.StencilReadMask = 0xFF;
    depthStencilDesc.StencilWriteMask = 0xFF;

    D3D11_DEPTH_STENCILOP_DESC& frontFace = depthStencilDesc.FrontFace;
    frontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
    frontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
    frontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
    frontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

    D3D11_DEPTH_STENCILOP_DESC& backFace = depthStencilDesc.BackFace;
    backFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
    backFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR;
    backFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
    backFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
    result = device->CreateDepthStencilState(&depthStencilDesc, depthStencilState.GetAddressOf());
    if (FAILED(result)) {
        return false;
    }

    deviceContext->OMSetDepthStencilState(depthStencilState.Get(), 1);
    
    D3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc = {};
    depthStencilViewDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
    depthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
    depthStencilViewDesc.Texture2D.MipSlice = 0;
    result = device->CreateDepthStencilView(depthStencilBuffer.Get(), &depthStencilViewDesc, depthStencilView.GetAddressOf());
    if (FAILED(result)) {
        return false;
    }
    deviceContext->OMSetRenderTargets(1, renderTargetView.GetAddressOf(), depthStencilView.Get());

    D3D11_RASTERIZER_DESC rasterizerDesc = {};
    rasterizerDesc.FillMode = D3D11_FILL_SOLID;
    rasterizerDesc.CullMode = D3D11_CULL_BACK;
    rasterizerDesc.FrontCounterClockwise = false;
    rasterizerDesc.DepthBias = 0;
    rasterizerDesc.DepthBiasClamp = 0.0f;
    rasterizerDesc.SlopeScaledDepthBias = 0.0f;
    rasterizerDesc.DepthClipEnable = true;
    rasterizerDesc.ScissorEnable = false;
    rasterizerDesc.MultisampleEnable = false;
    rasterizerDesc.AntialiasedLineEnable = false;
    result = device->CreateRasterizerState(&rasterizerDesc, rasterizerState.GetAddressOf());
    if (FAILED(result)) {
        return false;
    }
    deviceContext->RSSetState(rasterizerState.Get());

    viewport.Width = (float)screenWidth;
    viewport.Height = (float)screenHeight;
    viewport.MinDepth = 0.0f;
    viewport.MaxDepth = 1.0f;
    viewport.TopLeftX = 0.0f;
    viewport.TopLeftY = 0.0f;
    deviceContext->RSSetViewports(1, &viewport);

    setInitialized();
    return true;
}

void Direct3D::release() {
    if (isReleased()) {
        return;
    }

    if (swapChain) {
        swapChain->SetFullscreenState(false, NULL);
    }
    rasterizerState.Reset();
    depthStencilView.Reset();
    depthStencilState.Reset();
    depthStencilBuffer.Reset();
    renderTargetView.Reset();
    deviceContext.Reset();
    device.Reset();
    swapChain.Reset();

    vsyncEnabled = false;
    videoCardMemory = 0;

    viewport = {};

    setReleased();
}

void Direct3D::getVideoCardData(char* cardName, int& memory) {
    strcpy_s(cardName, sizeof(videoCardDescription), videoCardDescription);
    memory = videoCardMemory;
}

Microsoft::WRL::ComPtr<ID3D11Device> Direct3D::getDevice() {
    return device;
}

Microsoft::WRL::ComPtr<ID3D11DeviceContext> Direct3D::getDeviceContext() {
    return deviceContext;
}

void Direct3D::beginScene(DirectX::XMFLOAT4 color) {
    float colorBuffer[4] = { color.x, color.y, color.z, color.w };

    deviceContext->ClearRenderTargetView(renderTargetView.Get(), colorBuffer);
    deviceContext->ClearDepthStencilView(depthStencilView.Get(), D3D11_CLEAR_DEPTH, 1.0f, 0);
}

void Direct3D::endScene() {
    if (vsyncEnabled) {
        swapChain->Present(1, 0);
    }
    else {
        swapChain->Present(0, 0);
    }
}

void Direct3D::setBackBufferRenderTarget() {
    deviceContext->OMSetRenderTargets(1, renderTargetView.GetAddressOf(), depthStencilView.Get());
}

void Direct3D::resetViewport() {
    deviceContext->RSSetViewports(1, &viewport);
}
