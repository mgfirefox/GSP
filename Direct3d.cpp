#include "Direct3d.h"

Direct3d::Direct3d(std::shared_ptr<Window> window) : adapterData{}, swapChain(), device(),
                                                     deviceContext(),
                                                     renderTargetView(), depthStencilBuffer(),
                                                     depthStencilState(),
                                                     depthStencilView(), rasterizerState(),
                                                     viewport{}
{
    initialized = false;
    released = false;

    this->window = window;

    vsyncEnabled = false;
}

Direct3d::~Direct3d()
{
    release();

    window.reset();
}

bool Direct3d::isInitialized()
{
    return initialized;
}

void Direct3d::setInitialized()
{
    initialized = true;
    released = false;
}

bool Direct3d::isReleased()
{
    return released;
}

void Direct3d::setReleased()
{
    initialized = false;
    released = true;
}

AdapterData Direct3d::getAdapterData()
{
    return adapterData;
}

Microsoft::WRL::ComPtr<ID3D11Device> Direct3d::getDevice()
{
    return device;
}

Microsoft::WRL::ComPtr<ID3D11DeviceContext> Direct3d::getDeviceContext()
{
    return deviceContext;
}

bool Direct3d::initialize(bool vsyncEnabled)
{
    if (isInitialized())
    {
        release();
    }

    this->vsyncEnabled = vsyncEnabled;

    Microsoft::WRL::ComPtr<IDXGIAdapter> adapter;
    bool result = getAdapter(adapter);
    if (!result)
    {
        return false;
    }

    result = initializeAdapterData(adapter);
    if (!result)
    {
        return false;
    }

    DXGI_MODE_DESC displayModeDesc;
    result = createDisplayModeDesc(adapter, displayModeDesc);
    if (!result)
    {
        return false;
    }

    adapter.Reset();

    DXGI_SWAP_CHAIN_DESC swapChainDesc;

    result = initializeSwapChain(displayModeDesc, swapChainDesc);
    if (!result)
    {
        return false;
    }

    result = initializeRenderTargetView();
    if (!result)
    {
        return false;
    }

    result = initializeDepthStencilBuffer(swapChainDesc);
    if (!result)
    {
        return false;
    }

    result = initializeDepthStencilState();
    if (!result)
    {
        return false;
    }

    result = initializeDepthStencilView();
    if (!result)
    {
        return false;
    }

    result = initializeRasterizerState();
    if (!result)
    {
        return false;
    }

    result = initializeViewport();
    if (!result)
    {
        return false;
    }

    setInitialized();
    return true;
}

void Direct3d::release()
{
    if (isReleased())
    {
        return;
    }

    if (swapChain)
    {
        swapChain->SetFullscreenState(false, nullptr);
    }

    rasterizerState.Reset();
    depthStencilView.Reset();
    depthStencilState.Reset();
    depthStencilBuffer.Reset();

    renderTargetView.Reset();

    deviceContext.Reset();
    device.Reset();
    swapChain.Reset();

    adapterData = {};

    vsyncEnabled = false;

    viewport = {};

    setReleased();
}

bool Direct3d::createVertexShader(Microsoft::WRL::ComPtr<ID3D11VertexShader>& vertexShader,
                                  std::wstring vertexShaderFilename,
                                  Microsoft::WRL::ComPtr<ID3D10Blob>& vertexShaderBuffer)
{
    Microsoft::WRL::ComPtr<ID3D10Blob> errorMessage;

    HRESULT result = D3DCompileFromFile(vertexShaderFilename.c_str(), nullptr, nullptr,
                                        "VertexMain", "vs_5_0",
                                        D3DCOMPILE_ENABLE_STRICTNESS, 0,
                                        vertexShaderBuffer.GetAddressOf(),
                                        errorMessage.GetAddressOf());
    if (FAILED(result))
    {
        showShaderErrorMessage(errorMessage, vertexShaderFilename);

        return false;
    }

    result = device->CreateVertexShader(vertexShaderBuffer->GetBufferPointer(),
                                        vertexShaderBuffer->GetBufferSize(), nullptr,
                                        vertexShader.GetAddressOf());
    if (FAILED(result))
    {
        return false;
    }

    return true;
}

bool Direct3d::createPixelShader(Microsoft::WRL::ComPtr<ID3D11PixelShader>& pixelShader,
                                 std::wstring pixelShaderFilename)
{
    Microsoft::WRL::ComPtr<ID3D10Blob> errorMessage;
    Microsoft::WRL::ComPtr<ID3D10Blob> pixelShaderBuffer;

    HRESULT result = D3DCompileFromFile(pixelShaderFilename.c_str(), nullptr, nullptr, "PixelMain",
                                        "ps_5_0",
                                        D3DCOMPILE_ENABLE_STRICTNESS, 0,
                                        pixelShaderBuffer.GetAddressOf(),
                                        errorMessage.GetAddressOf());
    if (FAILED(result))
    {
        showShaderErrorMessage(errorMessage, pixelShaderFilename);

        return false;
    }

    result = device->CreatePixelShader(pixelShaderBuffer->GetBufferPointer(),
                                       pixelShaderBuffer->GetBufferSize(), nullptr,
                                       pixelShader.GetAddressOf());
    if (FAILED(result))
    {
        return false;
    }

    pixelShaderBuffer.Reset();

    return true;
}

void Direct3d::showShaderErrorMessage(Microsoft::WRL::ComPtr<ID3D10Blob> errorMessage,
                                      std::wstring shaderFilename)
{
    if (!errorMessage)
    {
        MessageBox(window->getHandle(), L"Missing Shader File", shaderFilename.c_str(), MB_OK);

        return;
    }

    auto compileErrors = static_cast<char*>(errorMessage->GetBufferPointer());
    uint64 bufferSize = errorMessage->GetBufferSize();

    std::wstringstream s;
    for (uint64 i = 0; i < bufferSize; i++)
    {
        s << compileErrors[i];
    }

    errorMessage.Reset();

    MessageBox(window->getHandle(), s.str().c_str(), shaderFilename.c_str(), MB_OK);
}

bool Direct3d::setVertexShader(Microsoft::WRL::ComPtr<ID3D11VertexShader> vertexShader)
{
    deviceContext->VSSetShader(vertexShader.Get(), nullptr, 0);

    return true;
}

bool Direct3d::setPixelShader(Microsoft::WRL::ComPtr<ID3D11PixelShader> pixelShader)
{
    deviceContext->PSSetShader(pixelShader.Get(), nullptr, 0);

    return true;
}

bool Direct3d::createInputLayout(Microsoft::WRL::ComPtr<ID3D11InputLayout>& inputLayout,
                                 const D3D11_INPUT_ELEMENT_DESC* inputElementDescs,
                                 uint32 inputElementDescCount,
                                 Microsoft::WRL::ComPtr<ID3D10Blob> vertexShaderBuffer)
{
    HRESULT result = device->CreateInputLayout(inputElementDescs, inputElementDescCount,
                                               vertexShaderBuffer->GetBufferPointer(),
                                               vertexShaderBuffer->GetBufferSize(),
                                               inputLayout.GetAddressOf());
    if (FAILED(result))
    {
        return false;
    }

    return true;
}

bool Direct3d::setInputLayoutToInputAssembler(Microsoft::WRL::ComPtr<ID3D11InputLayout> inputLayout)
{
    deviceContext->IASetInputLayout(inputLayout.Get());

    return true;
}

bool Direct3d::createConstantBuffer(Microsoft::WRL::ComPtr<ID3D11Buffer>& constantBuffer,
                                    D3D11_BUFFER_DESC constantBufferDesc,
                                    uint32& actualConstantBufferSize)
{
    if (constantBufferDesc.ByteWidth <= 0)
    {
        return false;
    }

    uint32 remainder = constantBufferDesc.ByteWidth % 16;
    uint32 additionalSize = remainder == 0 ? 0 : 16 - remainder;

    constantBufferDesc.ByteWidth += additionalSize;
    actualConstantBufferSize = constantBufferDesc.ByteWidth;

    HRESULT result = device->CreateBuffer(&constantBufferDesc, nullptr,
                                          constantBuffer.GetAddressOf());
    if (FAILED(result))
    {
        return false;
    }

    return true;
}

bool Direct3d::setConstantBufferToVertexShader(Microsoft::WRL::ComPtr<ID3D11Buffer> constantBuffer,
                                               uint32 slotIndex)
{
    deviceContext->VSSetConstantBuffers(slotIndex, 1, constantBuffer.GetAddressOf());

    return true;
}

bool Direct3d::setConstantBufferToPixelShader(Microsoft::WRL::ComPtr<ID3D11Buffer> constantBuffer,
                                              uint32 slotIndex)
{
    deviceContext->PSSetConstantBuffers(slotIndex, 1, constantBuffer.GetAddressOf());

    return true;
}

bool Direct3d::setConstantBufferData(Microsoft::WRL::ComPtr<ID3D11Buffer> constantBuffer,
                                     const void* constantBufferData, uint32
                                     constantBufferSize)
{
    D3D11_MAPPED_SUBRESOURCE mappedSubresource;

    HRESULT result = deviceContext->Map(constantBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0,
                                        &mappedSubresource);
    if (FAILED(result))
    {
        return false;
    }

    std::memcpy(mappedSubresource.pData, constantBufferData, constantBufferSize);

    deviceContext->Unmap(constantBuffer.Get(), 0);

    return true;
}

bool Direct3d::createSamplerState(Microsoft::WRL::ComPtr<ID3D11SamplerState>& samplerState,
                                  D3D11_SAMPLER_DESC samplerStateDesc)
{
    HRESULT result = device->CreateSamplerState(&samplerStateDesc, samplerState.GetAddressOf());
    if (FAILED(result))
    {
        return false;
    }

    return true;
}

bool Direct3d::setSamplerStateToPixelShader(Microsoft::WRL::ComPtr<ID3D11SamplerState> samplerState,
                                            uint32 slotIndex)
{
    deviceContext->PSSetSamplers(slotIndex, 1, samplerState.GetAddressOf());

    return true;
}

bool Direct3d::createVertexBuffer(Microsoft::WRL::ComPtr<ID3D11Buffer>& vertexBuffer,
                                  const void* vertexes, uint32 vertexStride, uint32 vertexCount)
{
    if (!vertexes)
    {
        return false;
    }
    if (vertexCount <= 0)
    {
        return false;
    }

    D3D11_BUFFER_DESC vertexBufferDesc = {};
    vertexBufferDesc.ByteWidth = vertexStride * vertexCount;
    vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
    vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    vertexBufferDesc.CPUAccessFlags = 0;
    vertexBufferDesc.MiscFlags = 0;
    vertexBufferDesc.StructureByteStride = 0;

    D3D11_SUBRESOURCE_DATA vertexBufferData = {};
    vertexBufferData.pSysMem = vertexes;
    vertexBufferData.SysMemPitch = 0;
    vertexBufferData.SysMemSlicePitch = 0;

    HRESULT result = device->CreateBuffer(&vertexBufferDesc, &vertexBufferData,
                                          vertexBuffer.GetAddressOf());
    if (FAILED(result))
    {
        return false;
    }

    return true;
}

bool Direct3d::setVertexBufferToInputAssembler(Microsoft::WRL::ComPtr<ID3D11Buffer> vertexBuffer,
                                               uint32 vertexStride)
{
    uint32 offset = 0;

    deviceContext->IASetVertexBuffers(0, 1, vertexBuffer.GetAddressOf(), &vertexStride, &offset);

    return true;
}

bool Direct3d::createIndexBuffer(Microsoft::WRL::ComPtr<ID3D11Buffer>& indexBuffer,
                                 const uint32* indexes, uint32 indexCount)
{
    if (!indexes)
    {
        return false;
    }
    if (indexCount <= 0)
    {
        return false;
    }

    D3D11_BUFFER_DESC indexBufferDesc = {};
    indexBufferDesc.ByteWidth = sizeof(uint32) * indexCount;
    indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
    indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
    indexBufferDesc.CPUAccessFlags = 0;
    indexBufferDesc.MiscFlags = 0;
    indexBufferDesc.StructureByteStride = 0;

    D3D11_SUBRESOURCE_DATA indexBufferData = {};
    indexBufferData.pSysMem = indexes;
    indexBufferData.SysMemPitch = 0;
    indexBufferData.SysMemSlicePitch = 0;

    HRESULT result = device->CreateBuffer(&indexBufferDesc, &indexBufferData,
                                          indexBuffer.GetAddressOf());
    if (FAILED(result))
    {
        return false;
    }

    return true;
}

bool Direct3d::setIndexBufferToInputAssembler(Microsoft::WRL::ComPtr<ID3D11Buffer> indexBuffer)
{
    deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    deviceContext->IASetIndexBuffer(indexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);

    return true;
}

bool Direct3d::canMipmapBeGenerated(DXGI_FORMAT format)
{
    uint32 formatSupport = 0;

    HRESULT result = device->CheckFormatSupport(format, &formatSupport);
    if (FAILED(result))
    {
        return false;
    }

    if (!(formatSupport & (D3D11_FORMAT_SUPPORT_MIP_AUTOGEN |
        D3D11_FORMAT_SUPPORT_RENDER_TARGET)))
    {
        return false;
    }

    return true;
}

bool Direct3d::createTexture2d(Microsoft::WRL::ComPtr<ID3D11Texture2D>& texture2d,
                               D3D11_TEXTURE2D_DESC texture2dDesc,
                               const D3D11_SUBRESOURCE_DATA* initialData)
{
    HRESULT result = device->CreateTexture2D(&texture2dDesc, initialData, texture2d.GetAddressOf());
    if (FAILED(result))
    {
        return false;
    }

    return true;
}

bool Direct3d::createShaderResourceView(
    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>& shaderResourceView,
    D3D11_SHADER_RESOURCE_VIEW_DESC shaderResourceViewDesc,
    Microsoft::WRL::ComPtr<ID3D11Texture2D> texture2d,
    bool shouldGenerateMipmaps)
{
    HRESULT result = device->CreateShaderResourceView(texture2d.Get(),
                                                      &shaderResourceViewDesc,
                                                      shaderResourceView.GetAddressOf());
    if (FAILED(result))
    {
        return false;
    }

    if (shouldGenerateMipmaps)
    {
        deviceContext->GenerateMips(shaderResourceView.Get());
    }

    return true;
}

bool Direct3d::setShaderResourceViewToPixelShader(
    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> shaderResourceView, uint32 slotIndex)
{
    deviceContext->PSSetShaderResources(slotIndex, 1, shaderResourceView.GetAddressOf());

    return true;
}

bool Direct3d::drawIndexed(uint32 indexCount)
{
    deviceContext->DrawIndexed(indexCount, 0, 0);

    return true;
}

void Direct3d::setBackBufferRenderTarget()
{
    deviceContext->OMSetRenderTargets(1, renderTargetView.GetAddressOf(), depthStencilView.Get());
}

void Direct3d::resetViewport()
{
    deviceContext->RSSetViewports(1, &viewport);
}

void Direct3d::onFrameStarted(DirectX::XMFLOAT4 color)
{
    float colorBuffer[4] = {color.x, color.y, color.z, color.w};

    deviceContext->ClearRenderTargetView(renderTargetView.Get(), colorBuffer);
    deviceContext->ClearDepthStencilView(depthStencilView.Get(), D3D11_CLEAR_DEPTH, 1.0f, 0);
}

void Direct3d::onFrameFinished()
{
    if (vsyncEnabled)
    {
        swapChain->Present(1, 0);
    }
    else
    {
        swapChain->Present(0, 0);
    }
}

bool Direct3d::getAdapter(Microsoft::WRL::ComPtr<IDXGIAdapter>& adapter)
{
    Microsoft::WRL::ComPtr<IDXGIFactory> factory;
    HRESULT result = CreateDXGIFactory(IID_IDXGIFactory,
                                       reinterpret_cast<void**>(factory.GetAddressOf()));
    if (FAILED(result))
    {
        return false;
    }

    result = factory->EnumAdapters(0, adapter.GetAddressOf());
    if (FAILED(result))
    {
        return false;
    }

    factory.Reset();
    return true;
}

bool Direct3d::initializeAdapterData(Microsoft::WRL::ComPtr<IDXGIAdapter> adapter)
{
    DXGI_ADAPTER_DESC adapterDesc;
    HRESULT result = adapter->GetDesc(&adapterDesc);
    if (FAILED(result))
    {
        return false;
    }

    adapterData.name = adapterDesc.Description;
    adapterData.dedicatedMemorySize = static_cast<int>(adapterDesc.DedicatedVideoMemory);

    return true;
}

bool Direct3d::createDisplayModeDesc(Microsoft::WRL::ComPtr<IDXGIAdapter> adapter,
                                     DXGI_MODE_DESC& displayModeDesc)
{
    Microsoft::WRL::ComPtr<IDXGIOutput> adapterOutput;
    HRESULT result = adapter->EnumOutputs(0, adapterOutput.GetAddressOf());
    if (FAILED(result))
    {
        return false;
    }

    displayModeDesc = {};
    displayModeDesc.Width = window->getWidth();
    displayModeDesc.Height = window->getHeight();

    DXGI_RATIONAL& refreshRate = displayModeDesc.RefreshRate;
    refreshRate.Numerator = 0;
    refreshRate.Denominator = 1;

    displayModeDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    displayModeDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
    displayModeDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;

    uint32 displayModeListSize = 0;

    if (!vsyncEnabled)
    {
        return true;
    }

    result = adapterOutput->GetDisplayModeList(displayModeDesc.Format, DXGI_ENUM_MODES_INTERLACED,
                                               &displayModeListSize, nullptr);
    if (FAILED(result))
    {
        return false;
    }

    std::vector<DXGI_MODE_DESC> displayModeList(displayModeListSize);
    result = adapterOutput->GetDisplayModeList(displayModeDesc.Format, DXGI_ENUM_MODES_INTERLACED,
                                               &displayModeListSize, displayModeList.data());
    if (FAILED(result))
    {
        return false;
    }

    for (uint32 i = 0; i < displayModeListSize; i++)
    {
        if (displayModeList[i].Width == window->getWidth()
            && displayModeList[i].Height == window->getHeight())
        {
            displayModeDesc.RefreshRate = displayModeList[i].RefreshRate;

            break;
        }
    }
    displayModeList.clear();
    displayModeList.shrink_to_fit();

    adapterOutput.Reset();

    return true;
}

bool Direct3d::initializeSwapChain(DXGI_MODE_DESC displayModeDesc,
                                   DXGI_SWAP_CHAIN_DESC& swapChainDesc)
{
    swapChainDesc = {};
    swapChainDesc.BufferDesc = displayModeDesc;

    DXGI_SAMPLE_DESC& sampleDesc = swapChainDesc.SampleDesc;
    sampleDesc.Count = 1;
    sampleDesc.Quality = 0;

    swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    swapChainDesc.BufferCount = 1;
    swapChainDesc.OutputWindow = window->getHandle();
    swapChainDesc.Windowed = !window->isFullscreenEnabled();
    swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
    swapChainDesc.Flags = 0;

    std::array<D3D_FEATURE_LEVEL, 1> featureLevel = {D3D_FEATURE_LEVEL_11_0};

    HRESULT result = D3D11CreateDeviceAndSwapChain(nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr, 0,
                                                   featureLevel.data(), featureLevel.size(),
                                                   D3D11_SDK_VERSION,
                                                   &swapChainDesc,
                                                   swapChain.GetAddressOf(), device.GetAddressOf(),
                                                   nullptr,
                                                   deviceContext.GetAddressOf());
    if (FAILED(result))
    {
        return false;
    }

    return true;
}

bool Direct3d::initializeRenderTargetView()
{
    Microsoft::WRL::ComPtr<ID3D11Texture2D> backBuffer;
    HRESULT result = swapChain->GetBuffer(0, IID_ID3D11Texture2D,
                                          reinterpret_cast<void**>(backBuffer.GetAddressOf()));
    if (FAILED(result))
    {
        return false;
    }

    result = device->CreateRenderTargetView(backBuffer.Get(), nullptr,
                                            renderTargetView.GetAddressOf());
    if (FAILED(result))
    {
        return false;
    }

    backBuffer.Reset();

    return true;
}

bool Direct3d::initializeDepthStencilBuffer(DXGI_SWAP_CHAIN_DESC swapChainDesc)
{
    D3D11_TEXTURE2D_DESC depthStencilBufferDesc = {};
    depthStencilBufferDesc.Width = window->getWidth();
    depthStencilBufferDesc.Height = window->getHeight();
    depthStencilBufferDesc.MipLevels = 1;
    depthStencilBufferDesc.ArraySize = 1;
    depthStencilBufferDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
    depthStencilBufferDesc.SampleDesc = swapChainDesc.SampleDesc;
    depthStencilBufferDesc.Usage = D3D11_USAGE_DEFAULT;
    depthStencilBufferDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
    depthStencilBufferDesc.CPUAccessFlags = 0;
    depthStencilBufferDesc.MiscFlags = 0;

    HRESULT result = device->CreateTexture2D(&depthStencilBufferDesc, nullptr,
                                             depthStencilBuffer.GetAddressOf());
    if (FAILED(result))
    {
        return false;
    }

    return true;
}

bool Direct3d::initializeDepthStencilState()
{
    D3D11_DEPTH_STENCIL_DESC depthStencilStateDesc = {};
    depthStencilStateDesc.DepthEnable = true;
    depthStencilStateDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
    depthStencilStateDesc.DepthFunc = D3D11_COMPARISON_LESS;
    depthStencilStateDesc.StencilEnable = true;
    depthStencilStateDesc.StencilReadMask = 0xFF;
    depthStencilStateDesc.StencilWriteMask = 0xFF;

    D3D11_DEPTH_STENCILOP_DESC& frontFace = depthStencilStateDesc.FrontFace;
    frontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
    frontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
    frontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
    frontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

    D3D11_DEPTH_STENCILOP_DESC& backFace = depthStencilStateDesc.BackFace;
    backFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
    backFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR;
    backFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
    backFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

    HRESULT result = device->CreateDepthStencilState(&depthStencilStateDesc,
                                                     depthStencilState.GetAddressOf());
    if (FAILED(result))
    {
        return false;
    }

    deviceContext->OMSetDepthStencilState(depthStencilState.Get(), 1);

    return true;
}

bool Direct3d::initializeDepthStencilView()
{
    D3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc = {};
    depthStencilViewDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
    depthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
    depthStencilViewDesc.Texture2D.MipSlice = 0;

    HRESULT result = device->CreateDepthStencilView(depthStencilBuffer.Get(),
                                                    &depthStencilViewDesc,
                                                    depthStencilView.GetAddressOf());
    if (FAILED(result))
    {
        return false;
    }

    deviceContext->OMSetRenderTargets(1, renderTargetView.GetAddressOf(), depthStencilView.Get());

    return true;
}

bool Direct3d::initializeRasterizerState()
{
    D3D11_RASTERIZER_DESC rasterizerStateDesc = {};
    rasterizerStateDesc.FillMode = D3D11_FILL_SOLID;
    rasterizerStateDesc.CullMode = D3D11_CULL_BACK;
    rasterizerStateDesc.FrontCounterClockwise = false;
    rasterizerStateDesc.DepthBias = 0;
    rasterizerStateDesc.DepthBiasClamp = 0.0f;
    rasterizerStateDesc.SlopeScaledDepthBias = 0.0f;
    rasterizerStateDesc.DepthClipEnable = true;
    rasterizerStateDesc.ScissorEnable = false;
    rasterizerStateDesc.MultisampleEnable = false;
    rasterizerStateDesc.AntialiasedLineEnable = false;

    HRESULT result = device->CreateRasterizerState(&rasterizerStateDesc,
                                                   rasterizerState.GetAddressOf());
    if (FAILED(result))
    {
        return false;
    }

    deviceContext->RSSetState(rasterizerState.Get());

    return true;
}

bool Direct3d::initializeViewport()
{
    viewport.Width = static_cast<float>(window->getWidth());
    viewport.Height = static_cast<float>(window->getHeight());
    viewport.MinDepth = 0.0f;
    viewport.MaxDepth = 1.0f;
    viewport.TopLeftX = 0.0f;
    viewport.TopLeftY = 0.0f;

    deviceContext->RSSetViewports(1, &viewport);

    return true;
}
