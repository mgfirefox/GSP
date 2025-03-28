#include "Mesh.h"

const std::function<void(Mesh*)> Mesh::deleter = [](Mesh* mesh) {
    mesh->release();
    delete mesh;
    };

Mesh::Mesh() : indexBuffer(), material() {
    initialized = false;
    released = false;
}

Mesh::~Mesh() {
    release();
}

bool Mesh::isInitialized() {
    return initialized;
}

void Mesh::setInitialized() {
    initialized = true;
    released = false;
}

bool Mesh::isReleased() {
    return released;
}

void Mesh::setReleased() {
    initialized = false;
    released = true;
}

void Mesh::initialize(std::shared_ptr<IndexBuffer> indexBuffer, std::shared_ptr<Material> material) {
    if (isInitialized()) {
        release();
    }

    this->indexBuffer = indexBuffer;

    this->material = material;

    setInitialized();
}

bool Mesh::render(Microsoft::WRL::ComPtr<ID3D11DeviceContext> deviceContext, std::unique_ptr<Shader>& shader) {
    indexBuffer->set(deviceContext);
    deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

    bool result = material->set(deviceContext, shader);
    if (!result) {
        return false;
    }

    shader->set(deviceContext);
    deviceContext->DrawIndexed(indexBuffer->getSize(), 0, 0);

    return true;
}

void Mesh::release() {
    if (isReleased()) {
        return;
    }
    
    material.reset();

    indexBuffer.reset();

    setReleased();
}
