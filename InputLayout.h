#pragma once
#include <d3d11.h>

#include <wrl/client.h>

class InputLayout {
    bool initialized;
    bool released;

    Microsoft::WRL::ComPtr<ID3D11InputLayout> inputLayout;

public:
    InputLayout();
    ~InputLayout();

private:
    bool isInitialized();
    void setInitialized();

    bool isReleased();
    void setReleased();

public:
    Microsoft::WRL::ComPtr<ID3D11InputLayout> getInputLayout();

    HRESULT initialize(Microsoft::WRL::ComPtr<ID3D11Device> device, const D3D11_INPUT_ELEMENT_DESC* inputElementsDescs, UINT inputElementsDescsQuantity, Microsoft::WRL::ComPtr<ID3D10Blob> vertexShaderBuffer);
    void set(Microsoft::WRL::ComPtr<ID3D11DeviceContext> deviceContext);
    void release();
};
