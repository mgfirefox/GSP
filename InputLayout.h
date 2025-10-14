#pragma once
#include <d3d11.h>

#include <wrl/client.h>
#include <memory>

#include "Direct3d.h"

#include "IntUtility.h"

class InputLayout
{
    bool initialized;
    bool released;

    std::shared_ptr<Direct3d> direct3d;

    Microsoft::WRL::ComPtr<ID3D11InputLayout> inputLayout;

public:
    InputLayout(std::shared_ptr<Direct3d> direct3d);
    ~InputLayout();

private:
    bool isInitialized();
    void setInitialized();

    bool isReleased();
    void setReleased();

public:
    Microsoft::WRL::ComPtr<ID3D11InputLayout> getInputLayout();

    bool initialize(const D3D11_INPUT_ELEMENT_DESC* inputElementDescs,
                    uint32 inputElementDescCount,
                    Microsoft::WRL::ComPtr<ID3D10Blob> vertexShaderBuffer);
    void release();
};
