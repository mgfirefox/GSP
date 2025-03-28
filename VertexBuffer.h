#pragma once
#include <d3d11.h>

#include <wrl/client.h>

template <class T>
class VertexBuffer {
    bool initialized;
    bool released;

    UINT size;

    Microsoft::WRL::ComPtr<ID3D11Buffer> vertexBuffer;

public:
    static const std::function<void(VertexBuffer<T>*)> deleter;

    VertexBuffer();
    ~VertexBuffer();

private:
    bool isInitialized();
    void setInitialized();

    bool isReleased();
    void setReleased();

public:
    UINT getSize();

    Microsoft::WRL::ComPtr<ID3D11Buffer> getVertexBuffer();

    bool initialize(Microsoft::WRL::ComPtr<ID3D11Device> device, const T* vertexes, UINT vertexesQuantity);
    void set(Microsoft::WRL::ComPtr<ID3D11DeviceContext> deviceContext);
    void release();
};

template <class T>
const std::function<void(VertexBuffer<T>*)> VertexBuffer<T>::deleter = [](VertexBuffer<T>* vertexBuffer) {
    vertexBuffer->release();
    delete vertexBuffer;
    };

template<class T>
inline VertexBuffer<T>::VertexBuffer() : vertexBuffer() {
    initialized = false;
    released = false;

    size = 0;
}

template<class T>
inline VertexBuffer<T>::~VertexBuffer() {
    release();
}

template<class T>
inline bool VertexBuffer<T>::isInitialized() {
    return initialized;
}

template<class T>
inline void VertexBuffer<T>::setInitialized() {
    initialized = true;
    released = false;
}

template<class T>
inline bool VertexBuffer<T>::isReleased() {
    return released;
}

template<class T>
inline void VertexBuffer<T>::setReleased() {
    initialized = false;
    released = true;
}

template<class T>
inline UINT VertexBuffer<T>::getSize() {
    return size;
}

template<class T>
inline Microsoft::WRL::ComPtr<ID3D11Buffer> VertexBuffer<T>::getVertexBuffer() {
    return vertexBuffer;
}

template<class T>
inline bool VertexBuffer<T>::initialize(Microsoft::WRL::ComPtr<ID3D11Device> device, const T* vertexes, UINT vertexesQuantity) {
    if (isInitialized()) {
        release();
    }

    if (!vertexes) {
        return false;
    }
    if (vertexesQuantity <= 0) {
        return false;
    }

    size = vertexesQuantity;

    D3D11_BUFFER_DESC vertexBufferDesc = {};
    vertexBufferDesc.ByteWidth = sizeof(T) * size;
    vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
    vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    vertexBufferDesc.CPUAccessFlags = 0;
    vertexBufferDesc.MiscFlags = 0;
    vertexBufferDesc.StructureByteStride = 0;

    D3D11_SUBRESOURCE_DATA vertexBufferData = {};
    vertexBufferData.pSysMem = (const void*)vertexes;
    vertexBufferData.SysMemPitch = 0;
    vertexBufferData.SysMemSlicePitch = 0;

    HRESULT result = device->CreateBuffer(&vertexBufferDesc, &vertexBufferData, vertexBuffer.GetAddressOf());
    if (FAILED(result)) {
        return false;
    }

    setInitialized();
    return true;
}

template<class T>
inline void VertexBuffer<T>::set(Microsoft::WRL::ComPtr<ID3D11DeviceContext> deviceContext) {
    UINT stride = sizeof(T);
    UINT offset = 0;

    deviceContext->IASetVertexBuffers(0, 1, vertexBuffer.GetAddressOf(), &stride, &offset);
}

template<class T>
inline void VertexBuffer<T>::release() {
    if (isReleased()) {
        return;
    }

    vertexBuffer.Reset();

    size = 0;

    setReleased();
}
