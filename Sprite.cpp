#include "Sprite.h"

#include <algorithm>

Sprite::Sprite(std::shared_ptr<Shader> shader, std::shared_ptr<Window> window,
               std::shared_ptr<Direct3d> direct3d) : vertexBuffer(), indexBuffer(),
                                                     diffuseColorTexture()
{
    initialized = false;
    released = false;

    this->shader = shader;

    this->window = window;

    this->direct3d = direct3d;

    transformation = Transformation::identity;
}

Sprite::Sprite(const Sprite& sprite)
{
    initialized = sprite.initialized;
    released = sprite.released;

    vertexBuffer = sprite.vertexBuffer;
    indexBuffer = sprite.indexBuffer;

    diffuseColorTexture = sprite.diffuseColorTexture;

    transformation = sprite.transformation;
}

Sprite::~Sprite()
{
    release();

    direct3d.reset();
}

bool Sprite::isInitialized()
{
    return initialized;
}

void Sprite::setInitialized()
{
    initialized = true;
    released = false;
}

bool Sprite::isReleased()
{
    return released;
}

void Sprite::setReleased()
{
    initialized = false;
    released = true;
}

Transformation Sprite::getTransformation()
{
    return transformation;
}

bool Sprite::initialize(std::string textureFilename, DirectX::XMFLOAT2 relativeSize)
{
    if (isInitialized())
    {
        release();
    }

    relativeSize.x = std::max(relativeSize.x, 0.0f);
    relativeSize.x = std::min(relativeSize.x, 1.0f);

    relativeSize.y = std::max(relativeSize.y, 0.0f);
    relativeSize.y = std::min(relativeSize.y, 1.0f);

    DirectX::XMFLOAT2 absoluteSize = {};
    absoluteSize.x = window->getWidth() * relativeSize.x;
    absoluteSize.y = window->getHeight() * relativeSize.y;

    float x = absoluteSize.x / 2;
    float y = absoluteSize.y / 2;

    std::array<Vertex, 4> vertexes;

    Vertex* vertex = &vertexes[0];
    vertex->position = DirectX::XMFLOAT3(-x, -y, 1.0f);
    vertex->textureCoordinates = DirectX::XMFLOAT3(0.0f, 1.0f, 0.0f);

    vertex = &vertexes[1];
    vertex->position = DirectX::XMFLOAT3(-x, y, 1.0f);
    vertex->textureCoordinates = DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f);

    vertex = &vertexes[2];
    vertex->position = DirectX::XMFLOAT3(x, -y, 1.0f);
    vertex->textureCoordinates = DirectX::XMFLOAT3(1.0f, 1.0f, 0.0f);

    vertex = &vertexes[3];
    vertex->position = DirectX::XMFLOAT3(x, y, 1.0f);
    vertex->textureCoordinates = DirectX::XMFLOAT3(1.0f, 0.0f, 0.0f);

    vertexBuffer = createSharedPointer<VertexBuffer<Vertex>>(direct3d);
    bool result = vertexBuffer->initialize(vertexes.data(), vertexes.size());
    if (!result)
    {
        return false;
    }

    std::array<uint32, 6> indexes = {0, 1, 2, 3, 2, 1};

    indexBuffer = createSharedPointer<IndexBuffer>(direct3d);
    result = indexBuffer->initialize(indexes.data(), indexes.size());
    if (!result)
    {
        return false;
    }

    diffuseColorTexture = createSharedPointer<Texture>(direct3d);
    result = diffuseColorTexture->initialize(textureFilename);
    if (!result)
    {
        return false;
    }

    this->transformation = transformation;

    setInitialized();
    return true;
}

bool Sprite::render(DirectX::XMMATRIX projectionMatrix)
{
    bool result = shader->setVertexBuffer(vertexBuffer);
    if (!result)
    {
        return false;
    }

    result = shader->setIndexBuffer(indexBuffer);
    if (!result)
    {
        return false;
    }

    DirectX::XMMATRIX modelMatrix = transformation.getTransformationMatrix();

    MvpBuffer mvpBuffer = {};
    mvpBuffer.mvpMatrix = DirectX::XMMatrixMultiply(modelMatrix, projectionMatrix);
    mvpBuffer.mvpMatrix = DirectX::XMMatrixTranspose(mvpBuffer.mvpMatrix);

    result = shader->setVertexShaderConstantBufferData(&mvpBuffer, VsMvpBufferSlotIndex);
    if (!result)
    {
        return false;
    }

    result = shader->setPixelShaderTexture(diffuseColorTexture,
                                           PsDiffuseColorTextureResourceSlotIndex);
    if (!result)
    {
        return false;
    }

    result = shader->render();
    if (!result)
    {
        return false;
    }

    return true;
}

void Sprite::release()
{
    if (isReleased())
    {
        return;
    }

    transformation = Transformation::identity;

    diffuseColorTexture.reset();

    indexBuffer.reset();
    vertexBuffer.reset();

    setReleased();
}
