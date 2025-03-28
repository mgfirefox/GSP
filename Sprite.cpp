#include "Sprite.h"

const std::function<void(Sprite*)> Sprite::deleter = [](Sprite* sprite) {
    sprite->release();
    delete sprite;
    };

Sprite::Sprite() : transformation(), vertexBuffer(), indexBuffer(), textureResource() {
    initialized = false;
    released = false;
}

Sprite::Sprite(const Sprite& sprite) {
    initialized = sprite.initialized;
    released = sprite.released;

    transformation = std::make_unique<Transformation>(*sprite.transformation);

    vertexBuffer = sprite.vertexBuffer;
    indexBuffer = sprite.indexBuffer;

    textureResource = sprite.textureResource;
}

Sprite::~Sprite() {
    release();
}

bool Sprite::isInitialized() {
    return initialized;
}

void Sprite::setInitialized() {
    initialized = true;
    released = false;
}

bool Sprite::isReleased() {
    return released;
}

void Sprite::setReleased() {
    initialized = false;
    released = true;
}

std::unique_ptr<Transformation>& Sprite::getTransformation() {
    return transformation;
}

bool Sprite::initialize(Microsoft::WRL::ComPtr<ID3D11Device> device, Microsoft::WRL::ComPtr<ID3D11DeviceContext> deviceContext, std::string textureFilename, DirectX::XMFLOAT2 relativeSize) {
    if (isInitialized()) {
        release();
    }

    std::vector<Vertex> vertexes;

    if (relativeSize.x < 0.0f) {
        relativeSize.x = 0.0f;
    }
    if (relativeSize.y < 0.0f) {
        relativeSize.y = 0.0f;
    }
    if (relativeSize.x > 1.0f) {
        relativeSize.x = 1.0f;
    }
    if (relativeSize.y > 1.0f) {
        relativeSize.y = 1.0f;
    }

    DirectX::XMFLOAT2 absoluteSize = {};
    absoluteSize.x = GetSystemMetrics(SM_CXSCREEN) * relativeSize.x;
    absoluteSize.y = GetSystemMetrics(SM_CYSCREEN) * relativeSize.y;

    float x = absoluteSize.x / 2;
    float y = absoluteSize.y / 2;

    Vertex vertex = {};
    vertex.position = DirectX::XMFLOAT3(-x, -y, 1.0f);
    vertex.textureCoordinates = DirectX::XMFLOAT3(0.0f, 1.0f, 0.0f);
    vertexes.push_back(vertex);

    vertex = {};
    vertex.position = DirectX::XMFLOAT3(-x, y, 1.0f);
    vertex.textureCoordinates = DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f);
    vertexes.push_back(vertex);

    vertex = {};
    vertex.position = DirectX::XMFLOAT3(x, -y, 1.0f);
    vertex.textureCoordinates = DirectX::XMFLOAT3(1.0f, 1.0f, 0.0f);
    vertexes.push_back(vertex);

    vertex = {};
    vertex.position = DirectX::XMFLOAT3(x, y, 1.0f);
    vertex.textureCoordinates = DirectX::XMFLOAT3(1.0f, 0.0f, 0.0f);
    vertexes.push_back(vertex);

    std::vector<unsigned int> indexes({ 0, 1, 2, 3, 2, 1 });

    vertexBuffer = std::shared_ptr<VertexBuffer<Vertex>>(new VertexBuffer<Vertex>, VertexBuffer<Vertex>::deleter);
    bool result = vertexBuffer->initialize(device, vertexes.data(), (UINT)vertexes.size());
    if (!result) {
        return false;
    }

    indexBuffer = std::shared_ptr<IndexBuffer>(new IndexBuffer, IndexBuffer::deleter);
    result = indexBuffer->initialize(device, indexes.data(), (UINT)indexes.size());
    if (!result) {
        return false;
    }

    D3D11_TEXTURE2D_DESC texture2DDesc = {};

    DXGI_SAMPLE_DESC& sampleDesc = texture2DDesc.SampleDesc;
    sampleDesc.Count = 1;
    sampleDesc.Quality = 0;

    bool shouldGenerateMipmaps = false;

    D3D11_SHADER_RESOURCE_VIEW_DESC textureResourceDesc = {};

    std::shared_ptr<Texture2D> texture(new Texture2D, Texture2D::deleter);
    result = texture->initialize(device, deviceContext, textureFilename, texture2DDesc, shouldGenerateMipmaps, textureResourceDesc.Format);
    if (!result) {
        return false;
    }

    textureResourceDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;

    D3D11_TEX2D_SRV& texture2D = textureResourceDesc.Texture2D;
    texture2D.MostDetailedMip = 0;
    texture2D.MipLevels = -1;

    textureResource = std::shared_ptr<ShaderResource>(new ShaderResource, ShaderResource::deleter);
    result = textureResource->initialize(device, deviceContext, textureResourceDesc, texture, shouldGenerateMipmaps);
    if (!result) {
        return false;
    }

    this->transformation = std::make_unique<Transformation>();

    setInitialized();
    return true;
}

void Sprite::render(Microsoft::WRL::ComPtr<ID3D11DeviceContext> deviceContext, std::unique_ptr<Shader>& shader) {
    vertexBuffer->set(deviceContext);
    indexBuffer->set(deviceContext);
    deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

    textureResource->setPs(deviceContext, PS_TEXTURE_RESOURCE_SLOT);

    shader->set(deviceContext);
    deviceContext->DrawIndexed(indexBuffer->getSize(), 0, 0);
}

void Sprite::release() {
    if (isReleased()) {
        return;
    }

    textureResource.reset();

    indexBuffer.reset();
    vertexBuffer.reset();

    transformation.reset();

    setReleased();
}
