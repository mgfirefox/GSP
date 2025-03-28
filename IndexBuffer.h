#pragma once
#include <d3d11.h>

#include <wrl/client.h>

#include <functional>

class IndexBuffer {
    bool initialized;
    bool released;

    UINT size;

    Microsoft::WRL::ComPtr<ID3D11Buffer> indexBuffer;

public:
    static const std::function<void(IndexBuffer*)> deleter;

    IndexBuffer();
    ~IndexBuffer();

private:
    bool isInitialized();
    void setInitialized();

    bool isReleased();
    void setReleased();

public:
    UINT getSize();

    Microsoft::WRL::ComPtr<ID3D11Buffer> getIndexBuffer();

    bool initialize(Microsoft::WRL::ComPtr<ID3D11Device> device, const unsigned int* indexes, UINT indexesQuantity);
    void set(Microsoft::WRL::ComPtr<ID3D11DeviceContext> deviceContext);
    void release();
};
