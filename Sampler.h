#pragma once
#include <d3d11.h>

#include <wrl/client.h>
#include <memory>

#include "Direct3d.h"

class Sampler
{
    bool initialized;
    bool released;

    std::shared_ptr<Direct3d> direct3d;

    Microsoft::WRL::ComPtr<ID3D11SamplerState> state;

public:
    Sampler(std::shared_ptr<Direct3d> direct3d);
    ~Sampler();

private:
    bool isInitialized();
    void setInitialized();

    bool isReleased();
    void setReleased();

public:
    Microsoft::WRL::ComPtr<ID3D11SamplerState> getState();

    bool initialize();
    void release();
};
