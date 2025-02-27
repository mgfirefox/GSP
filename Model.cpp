#include "Model.h"

Model::Model() : vertexBuffer(), indexBuffer(), vertexes(), indexes(), transformation{} {
    initialized = false;
    released = false;

    vertexesQuantity = 0;
    indexesQuantity = 0;
}

Model::Model(const Model& model) {
    initialized = model.initialized;
    released = model.released;

    vertexBuffer = model.vertexBuffer;
    indexBuffer = model.indexBuffer;

    vertexes = model.vertexes;
    indexes = model.indexes;

    vertexesQuantity = model.vertexesQuantity;
    indexesQuantity = model.indexesQuantity;

    transformation = model.transformation;
}

Model::~Model() {
    release();
}

bool Model::isInitialized() {
    return initialized;
}

void Model::setInitialized() {
    initialized = true;
    released = false;
}

bool Model::isReleased() {
    return released;
}

void Model::setReleased() {
    initialized = false;
    released = true;
}

Transformation Model::getTransformation() {
    return transformation;
}

void Model::setTransformation(Transformation transformation) {
    this->transformation = transformation;
}

bool Model::initialize(Microsoft::WRL::ComPtr<ID3D11Device> device, std::string filename, Transformation transformation) {
    if (isInitialized()) {
        release();
    }

    bool result = loadModel(filename);
    if (!result) {
        return false;
    }

    vertexesQuantity = (unsigned long)vertexes.size();
    indexesQuantity = (unsigned long)indexes.size();

    result = initializeBuffers(device);
    if (!result) {
        return false;
    }

    indexes.clear();
    vertexes.clear();

    setInitialized();
    return true;
}

void Model::render(Microsoft::WRL::ComPtr<ID3D11DeviceContext> deviceContext) {
    renderBuffers(deviceContext);
}

void Model::release() {
    if (isReleased()) {
        return;
    }

    releaseBuffers();
    releaseModel();

    setReleased();
}

bool Model::loadModel(std::string filename) {
    if (vertexesQuantity != 0 || indexesQuantity != 0) {
        releaseModel();
    }

    return ml::loadObj(filename, vertexes, indexes);
}

void Model::releaseModel() {
    vertexesQuantity = 0;
    indexesQuantity = 0;
}

bool Model::initializeBuffers(Microsoft::WRL::ComPtr<ID3D11Device> device) {
    D3D11_BUFFER_DESC vertexBufferDesc = {};
    vertexBufferDesc.ByteWidth = (unsigned int)(sizeof(vertexes[0]) * vertexesQuantity);
    vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
    vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    vertexBufferDesc.CPUAccessFlags = 0;
    vertexBufferDesc.MiscFlags = 0;
    vertexBufferDesc.StructureByteStride = 0;

    D3D11_SUBRESOURCE_DATA vertexBufferData = {};
    vertexBufferData.pSysMem = vertexes.data();
    vertexBufferData.SysMemPitch = 0;
    vertexBufferData.SysMemSlicePitch = 0;

    HRESULT result = device->CreateBuffer(&vertexBufferDesc, &vertexBufferData, vertexBuffer.GetAddressOf());
    
    if (FAILED(result)) {
        return false;
    }

    D3D11_BUFFER_DESC indexBufferDesc = {};
    indexBufferDesc.ByteWidth = (unsigned int)(sizeof(indexes[0]) * indexesQuantity);
    indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
    indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
    indexBufferDesc.CPUAccessFlags = 0;
    indexBufferDesc.MiscFlags = 0;
    indexBufferDesc.StructureByteStride = 0;

    D3D11_SUBRESOURCE_DATA indexBufferData = {};
    indexBufferData.pSysMem = (void*)indexes.data();
    indexBufferData.SysMemPitch = 0;
    indexBufferData.SysMemSlicePitch = 0;

    result = device->CreateBuffer(&indexBufferDesc, &indexBufferData, indexBuffer.GetAddressOf());
    
    if (FAILED(result)) {
        return false;
    }

    return true;
}

void Model::renderBuffers(Microsoft::WRL::ComPtr<ID3D11DeviceContext> deviceContext) {
    unsigned int stride = sizeof(ml::Vertex);
    unsigned int offset = 0;

    deviceContext->IASetVertexBuffers(0, 1, vertexBuffer.GetAddressOf(), &stride, &offset);
    deviceContext->IASetIndexBuffer(indexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);
    deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

    deviceContext->DrawIndexed((unsigned int)indexesQuantity, 0, 0);
}

void Model::releaseBuffers() {
    indexBuffer.Reset();
    vertexBuffer.Reset();
}
