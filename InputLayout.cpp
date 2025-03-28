#include "InputLayout.h"

InputLayout::InputLayout() : inputLayout() {
    initialized = false;
    released = false;
}

InputLayout::~InputLayout() {
    release();
}

bool InputLayout::isInitialized() {
    return initialized;
}

void InputLayout::setInitialized() {
    initialized = true;
    released = false;
}

bool InputLayout::isReleased() {
    return released;
}

void InputLayout::setReleased() {
    initialized = false;
    released = true;
}

Microsoft::WRL::ComPtr<ID3D11InputLayout> InputLayout::getInputLayout() {
    return inputLayout;
}

HRESULT InputLayout::initialize(Microsoft::WRL::ComPtr<ID3D11Device> device, const D3D11_INPUT_ELEMENT_DESC* inputElementsDescs, UINT inputElementsDescsQuantity, Microsoft::WRL::ComPtr<ID3D10Blob> vertexShaderBuffer) {
    return device->CreateInputLayout(inputElementsDescs, inputElementsDescsQuantity, vertexShaderBuffer->GetBufferPointer(), vertexShaderBuffer->GetBufferSize(), inputLayout.GetAddressOf());
}

void InputLayout::set(Microsoft::WRL::ComPtr<ID3D11DeviceContext> deviceContext) {
    deviceContext->IASetInputLayout(inputLayout.Get());
}

void InputLayout::release() {
    if (isReleased()) {
        return;
    }

    inputLayout.Reset();

    setReleased();
}
