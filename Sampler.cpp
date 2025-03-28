#include "Sampler.h"

const std::function<void(Sampler*)> Sampler::deleter = [](Sampler* sampler) {
    sampler->release();
    delete sampler;
    };

Sampler::Sampler() : samplerState() {
    initialized = false;
    released = false;
}

Sampler::~Sampler() {
    release();
}

bool Sampler::isInitialized() {
    return initialized;
}

void Sampler::setInitialized() {
    initialized = true;
    released = false;
}

bool Sampler::isReleased() {
    return released;
}

void Sampler::setReleased() {
    initialized = false;
    released = true;
}

Microsoft::WRL::ComPtr<ID3D11SamplerState> Sampler::getSamplerState() {
    return samplerState;
}

bool Sampler::initialize(Microsoft::WRL::ComPtr<ID3D11Device> device, D3D11_SAMPLER_DESC samplerStateDesc) {
    if (isInitialized()) {
        release();
    }
    
    HRESULT result = device->CreateSamplerState(&samplerStateDesc, samplerState.GetAddressOf());
    if (FAILED(result)) {
        return false;
    }

    setInitialized();
    return true;
}

void Sampler::setPs(Microsoft::WRL::ComPtr<ID3D11DeviceContext> deviceContext, UINT slot) {
    deviceContext->PSSetSamplers(slot, 1, samplerState.GetAddressOf());
}

void Sampler::release() {
    if (isReleased()) {
        return;
    }

    samplerState.Reset();

    setReleased();
}
