#pragma once
#include <d3d11.h>

#include <wrl/client.h>

#include <functional>

class Sampler {
    bool initialized;
    bool released;

    Microsoft::WRL::ComPtr<ID3D11SamplerState> samplerState;

public:
    static const std::function<void(Sampler*)> deleter;

    Sampler();
    ~Sampler();

private:
    bool isInitialized();
    void setInitialized();

    bool isReleased();
    void setReleased();

public:
    Microsoft::WRL::ComPtr<ID3D11SamplerState> getSamplerState();

    bool initialize(Microsoft::WRL::ComPtr<ID3D11Device> device, D3D11_SAMPLER_DESC samplerStateDesc);
    void setPs(Microsoft::WRL::ComPtr<ID3D11DeviceContext> deviceContext, UINT slot);
    void release();
};
