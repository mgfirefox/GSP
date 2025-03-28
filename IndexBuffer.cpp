#include "IndexBuffer.h"

const std::function<void(IndexBuffer*)> IndexBuffer::deleter = [](IndexBuffer* indexBuffer) {
    indexBuffer->release();
    delete indexBuffer;
    };

IndexBuffer::IndexBuffer() : indexBuffer() {
    initialized = false;
    released = false;

    size = 0;
}

IndexBuffer::~IndexBuffer() {
    release();
}

bool IndexBuffer::isInitialized() {
    return initialized;
}

void IndexBuffer::setInitialized() {
    initialized = true;
    released = false;
}

bool IndexBuffer::isReleased() {
    return released;
}

void IndexBuffer::setReleased() {
    initialized = false;
    released = true;
}

UINT IndexBuffer::getSize() {
    return size;
}

Microsoft::WRL::ComPtr<ID3D11Buffer> IndexBuffer::getIndexBuffer() {
    return indexBuffer;
}

bool IndexBuffer::initialize(Microsoft::WRL::ComPtr<ID3D11Device> device, const unsigned int* indexes, UINT indexesQuantity) {
    if (isInitialized()) {
        release();
    }
    
    if (!indexes) {
        return false;
    }
    if (indexesQuantity <= 0) {
        return false;
    }

    size = indexesQuantity;

    D3D11_BUFFER_DESC indexBufferDesc = {};
    indexBufferDesc.ByteWidth = sizeof(unsigned int) * size;
    indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
    indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
    indexBufferDesc.CPUAccessFlags = 0;
    indexBufferDesc.MiscFlags = 0;
    indexBufferDesc.StructureByteStride = 0;

    D3D11_SUBRESOURCE_DATA indexBufferData = {};
    indexBufferData.pSysMem = (const void*)indexes;
    indexBufferData.SysMemPitch = 0;
    indexBufferData.SysMemSlicePitch = 0;

    HRESULT result = device->CreateBuffer(&indexBufferDesc, &indexBufferData, indexBuffer.GetAddressOf());
    if (FAILED(result)) {
        return false;
    }

    setInitialized();
    return true;
}

void IndexBuffer::set(Microsoft::WRL::ComPtr<ID3D11DeviceContext> deviceContext) {
    deviceContext->IASetIndexBuffer(indexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);
}

void IndexBuffer::release() {
    if (isReleased()) {
        return;
    }

    indexBuffer.Reset();

    size = 0;

    setReleased();
}
