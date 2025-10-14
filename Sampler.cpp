#include "Sampler.h"

Sampler::Sampler(std::shared_ptr<Direct3d> direct3d) : state()
{
    initialized = false;
    released = false;

    this->direct3d = direct3d;
}

Sampler::~Sampler()
{
    release();

    direct3d.reset();
}

bool Sampler::isInitialized()
{
    return initialized;
}

void Sampler::setInitialized()
{
    initialized = true;
    released = false;
}

bool Sampler::isReleased()
{
    return released;
}

void Sampler::setReleased()
{
    initialized = false;
    released = true;
}

Microsoft::WRL::ComPtr<ID3D11SamplerState> Sampler::getState()
{
    return state;
}

bool Sampler::initialize()
{
    if (isInitialized())
    {
        release();
    }

    D3D11_SAMPLER_DESC samplerStateDesc = {};
    samplerStateDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
    samplerStateDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
    samplerStateDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
    samplerStateDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
    samplerStateDesc.MipLODBias = 0.0f;
    samplerStateDesc.MaxAnisotropy = 1;
    samplerStateDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;

    float* borderColor = samplerStateDesc.BorderColor;
    borderColor[0] = 0.0f;
    borderColor[1] = 0.0f;
    borderColor[2] = 0.0f;
    borderColor[3] = 0.0f;

    samplerStateDesc.MinLOD = 0;
    samplerStateDesc.MaxLOD = D3D11_FLOAT32_MAX;

    bool result = direct3d->createSamplerState(state, samplerStateDesc);
    if (!result)
    {
        return false;
    }

    setInitialized();
    return true;
}

void Sampler::release()
{
    if (isReleased())
    {
        return;
    }

    state.Reset();

    setReleased();
}
