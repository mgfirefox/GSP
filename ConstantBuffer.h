#pragma once
#include "BaseConstantBuffer.h"

struct MvpBuffer {
    DirectX::XMMATRIX mvpMatrix;
};

struct MaterialBuffer {
    DirectX::XMFLOAT3 diffuseColor;
    float opacity;

    bool hasDiffuseColorTexture;
};

template<class T>
class ConstantBuffer : public BaseConstantBuffer {
    bool initialized;
    bool released;

public:
    ConstantBuffer();
    ~ConstantBuffer();

private:
    bool isInitialized();
    void setInitialized();

    bool isReleased();
    void setReleased();

public:
    Microsoft::WRL::ComPtr<ID3D11Buffer> getConstantBuffer();

    bool setData(Microsoft::WRL::ComPtr<ID3D11DeviceContext> deviceContext, T data);

    bool initialize(Microsoft::WRL::ComPtr<ID3D11Device> device, D3D11_BUFFER_DESC constantBufferDesc);
    void setVs(Microsoft::WRL::ComPtr<ID3D11DeviceContext> deviceContext, UINT slot);
    void setPs(Microsoft::WRL::ComPtr<ID3D11DeviceContext> deviceContext, UINT slot);
    void release();
};

template<class T>
inline ConstantBuffer<T>::ConstantBuffer() : BaseConstantBuffer() {
    initialized = false;
    released = false;
}

template<class T>
inline ConstantBuffer<T>::~ConstantBuffer() {
    release();
}

template<class T>
inline bool ConstantBuffer<T>::isInitialized() {
    return initialized;
}

template<class T>
inline void ConstantBuffer<T>::setInitialized() {
    initialized = true;
    released = false;
}

template<class T>
inline bool ConstantBuffer<T>::isReleased() {
    return released;
}

template<class T>
inline void ConstantBuffer<T>::setReleased() {
    initialized = false;
    released = true;
}

template<class T>
inline Microsoft::WRL::ComPtr<ID3D11Buffer> ConstantBuffer<T>::getConstantBuffer() {
    return constantBuffer;
}

template<class T>
inline bool ConstantBuffer<T>::setData(Microsoft::WRL::ComPtr<ID3D11DeviceContext> deviceContext, T data) {
    D3D11_MAPPED_SUBRESOURCE mappedSubresource;
    HRESULT result = deviceContext->Map(constantBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedSubresource);
    if (FAILED(result)) {
        return false;
    }
    std::memcpy(mappedSubresource.pData, &data, sizeof(T));
    deviceContext->Unmap(constantBuffer.Get(), 0);

    return true;
}

template<class T>
inline bool ConstantBuffer<T>::initialize(Microsoft::WRL::ComPtr<ID3D11Device> device, D3D11_BUFFER_DESC constantBufferDesc) {
    if (isInitialized()) {
        release();
    }

    if (constantBufferDesc.ByteWidth <= 0) {
        return false;
    }

    UINT remainder = constantBufferDesc.ByteWidth % 16;
    UINT additionalSize = (remainder == 0) ? 0 : 16 - remainder;
    constantBufferDesc.ByteWidth += additionalSize;
    
    HRESULT result = device->CreateBuffer(&constantBufferDesc, NULL, constantBuffer.GetAddressOf());
    if (FAILED(result)) {
        return false;
    }

    setInitialized();
    return true;
}

template<class T>
inline void ConstantBuffer<T>::setVs(Microsoft::WRL::ComPtr<ID3D11DeviceContext> deviceContext, UINT slot) {
    deviceContext->VSSetConstantBuffers(slot, 1, constantBuffer.GetAddressOf());
}

template<class T>
inline void ConstantBuffer<T>::setPs(Microsoft::WRL::ComPtr<ID3D11DeviceContext> deviceContext, UINT slot) {
    deviceContext->PSSetConstantBuffers(slot, 1, constantBuffer.GetAddressOf());
}

template<class T>
inline void ConstantBuffer<T>::release() {
    if (isReleased()) {
        return;
    }

    BaseConstantBuffer::release();

    setReleased();
}
