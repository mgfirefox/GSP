#include "Shader.h"

Shader::Shader() : vertexShader(), pixelShader(), inputLayout(), vsConstantBuffers(), psConstantBuffers(), psSamplers() {
    initialized = false;
    released = false;
}

Shader::~Shader() {
    release();
}

bool Shader::isInitialized() {
    return initialized;
}

void Shader::setInitialized() {
    initialized = true;
    released = false;
}

bool Shader::isReleased() {
    return released;
}

void Shader::setReleased() {
    initialized = false;
    released = true;
}

void Shader::setPsSampler(UINT slot, std::shared_ptr<Sampler> sampler) {
    psSamplers[slot] = sampler;
}

void Shader::setPsShaderResource(UINT slot, std::shared_ptr<ShaderResource> shaderResource) {
    psShaderResources[slot] = shaderResource;
}

bool Shader::initialize(Microsoft::WRL::ComPtr<ID3D11Device> device, HWND windowHandle, std::wstring filename, const D3D11_INPUT_ELEMENT_DESC* inputElementsDesc, UINT inputElementsDescsQuantity) {
    if (isInitialized()) {
        release();
    }

    Microsoft::WRL::ComPtr<ID3D10Blob> errorMessage;
    Microsoft::WRL::ComPtr<ID3D10Blob> vertexShaderBuffer;
    HRESULT result = D3DCompileFromFile(filename.c_str(), NULL, NULL, "VertexMain", "vs_5_0", D3DCOMPILE_ENABLE_STRICTNESS, 0, vertexShaderBuffer.GetAddressOf(), errorMessage.GetAddressOf());
    if (FAILED(result)) {
        if (errorMessage) {
            outputShaderErrorMessage(errorMessage, windowHandle, filename);
        }
        else {
            MessageBox(windowHandle, filename.c_str(), L"Missing Shader File", MB_OK);
        }

        return false;
    }

    Microsoft::WRL::ComPtr<ID3D10Blob> pixelShaderBuffer;
    result = D3DCompileFromFile(filename.c_str(), NULL, NULL, "PixelMain", "ps_5_0", D3DCOMPILE_ENABLE_STRICTNESS, 0, pixelShaderBuffer.GetAddressOf(), errorMessage.GetAddressOf());
    if (FAILED(result)) {
        if (errorMessage) {
            outputShaderErrorMessage(errorMessage, windowHandle, filename);
        }
        else {
            MessageBox(windowHandle, filename.c_str(), L"Missing Shader File", MB_OK);
        }

        return false;
    }

    result = device->CreateVertexShader(vertexShaderBuffer->GetBufferPointer(), vertexShaderBuffer->GetBufferSize(), NULL, vertexShader.GetAddressOf());
    if (FAILED(result)) {
        return false;
    }

    result = device->CreatePixelShader(pixelShaderBuffer->GetBufferPointer(), pixelShaderBuffer->GetBufferSize(), NULL, pixelShader.GetAddressOf());
    if (FAILED(result)) {
        return false;
    }

    inputLayout = std::make_unique<InputLayout>();
    result = inputLayout->initialize(device, inputElementsDesc, inputElementsDescsQuantity, vertexShaderBuffer);
    if (FAILED(result)) {
        return false;
    }

    vertexShaderBuffer.Reset();
    pixelShaderBuffer.Reset();

    setInitialized();
    return true;
}

void Shader::set(Microsoft::WRL::ComPtr<ID3D11DeviceContext> deviceContext) {
    inputLayout->set(deviceContext);

    for (auto& pair : vsConstantBuffers) {
        pair.second->setVs(deviceContext, pair.first);
    }
    deviceContext->VSSetShader(vertexShader.Get(), NULL, 0);

    for (auto& pair : psConstantBuffers) {
        pair.second->setPs(deviceContext, pair.first);
    }
    for (auto& pair : psSamplers) {
        pair.second->setPs(deviceContext, pair.first);
    }
    for (auto& pair : psShaderResources) {
        pair.second->setPs(deviceContext, pair.first);
    }
    deviceContext->PSSetShader(pixelShader.Get(), NULL, 0);
}

void Shader::release() {
    if (isReleased()) {
        return;
    }

    for (auto& pair : psShaderResources) {
        pair.second.reset();
    }
    psShaderResources.clear();

    for (auto& pair : psSamplers) {
        pair.second.reset();
    }
    psSamplers.clear();

    for (auto& pair : psConstantBuffers) {
        pair.second.reset();
    }
    psConstantBuffers.clear();

    for (auto& pair : vsConstantBuffers) {
        pair.second.reset();
    }
    vsConstantBuffers.clear();

    inputLayout.reset();

    pixelShader.Reset();
    vertexShader.Reset();

    setReleased();
}

void Shader::outputShaderErrorMessage(Microsoft::WRL::ComPtr<ID3D10Blob> errorMessage, HWND windowHandle, std::wstring shaderFilename) {
    char* compileErrors = (char*)(errorMessage->GetBufferPointer());
    unsigned long long bufferSize = errorMessage->GetBufferSize();

    std::wofstream file;
    file.open("shader-error.txt");
    if (file.is_open()) {
        for (unsigned long long i = 0; i < bufferSize; i++) {
            file << compileErrors[i];
        }
    }
    file.close();

    errorMessage.Reset();

    MessageBox(windowHandle, L"Compiling Shader Error. Check shader-error.txt for more information", shaderFilename.c_str(), MB_OK);
}
