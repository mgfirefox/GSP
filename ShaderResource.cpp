#include "ShaderResource.h"

const std::function<void(ShaderResource*)> ShaderResource::deleter = [](ShaderResource* shaderResource) {
    shaderResource->release();
    delete shaderResource;
    };

ShaderResource::ShaderResource() : shaderResourceView() {
    initialized = false;
    released = false;
}

ShaderResource::~ShaderResource() {
    release();
}

bool ShaderResource::isInitialized() {
    return initialized;
}

void ShaderResource::setInitialized() {
    initialized = true;
    released = false;
}

bool ShaderResource::isReleased() {
    return released;
}

void ShaderResource::setReleased() {
    initialized = false;
    released = true;
}

Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> ShaderResource::getShaderResource() {
    return shaderResourceView;
}

bool ShaderResource::initialize(Microsoft::WRL::ComPtr<ID3D11Device> device, Microsoft::WRL::ComPtr<ID3D11DeviceContext> deviceContext, D3D11_SHADER_RESOURCE_VIEW_DESC shaderResourceViewDesc, std::shared_ptr<Texture2D> texture2D, bool shouldGenerateMipmaps) {
    if (isInitialized()) {
        release();
    }

    HRESULT result = device->CreateShaderResourceView(texture2D->getTexture2D().Get(), &shaderResourceViewDesc, shaderResourceView.GetAddressOf());
    if (FAILED(result)) {
        return false;
    }

    if (shouldGenerateMipmaps) {
        deviceContext->GenerateMips(shaderResourceView.Get());
    }

    setInitialized();
    return true;
}

void ShaderResource::setPs(Microsoft::WRL::ComPtr<ID3D11DeviceContext> deviceContext, UINT slot) {
    deviceContext->PSSetShaderResources(slot, 1, shaderResourceView.GetAddressOf());
}

void ShaderResource::release() {
    if (isReleased()) {
        return;
    }

    shaderResourceView.Reset();

    setReleased();
}
