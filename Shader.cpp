#include "Shader.h"

Shader::Shader(std::shared_ptr<Direct3d> direct3d) : vertexShader(), pixelShader(), inputLayout(),
                                                     vertexShaderConstantBuffers(),
                                                     pixelShaderConstantBuffers(),
                                                     pixelShaderSamplers()
{
    initialized = false;
    released = false;

    this->direct3d = direct3d;
}

Shader::~Shader()
{
    release();

    direct3d.reset();
}

bool Shader::isInitialized()
{
    return initialized;
}

void Shader::setInitialized()
{
    initialized = true;
    released = false;
}

bool Shader::isReleased()
{
    return released;
}

void Shader::setReleased()
{
    initialized = false;
    released = true;
}

bool Shader::setVertexShaderConstantBuffer(std::shared_ptr<AbstractConstantBuffer> constantBuffer,
                                           uint32 slotIndex)
{
    vertexShaderConstantBuffers[slotIndex] = constantBuffer;

    return true;
}

bool Shader::setPixelShaderConstantBuffer(std::shared_ptr<AbstractConstantBuffer> constantBuffer,
                                          uint32 slotIndex)
{
    pixelShaderConstantBuffers[slotIndex] = constantBuffer;

    return true;
}

bool Shader::setPixelShaderSampler(std::shared_ptr<Sampler> sampler, uint32 slotIndex)
{
    pixelShaderSamplers[slotIndex] = sampler;

    return true;
}

bool Shader::initialize(std::wstring filename, const D3D11_INPUT_ELEMENT_DESC* inputElementDescs,
                        uint32 inputElementDescCount)
{
    if (isInitialized())
    {
        release();
    }

    Microsoft::WRL::ComPtr<ID3D10Blob> vertexShaderBuffer;

    bool result = direct3d->createVertexShader(vertexShader, filename, vertexShaderBuffer);
    if (!result)
    {
        return false;
    }

    inputLayout = createSharedPointer<InputLayout>(direct3d);
    result = inputLayout->initialize(inputElementDescs, inputElementDescCount, vertexShaderBuffer);
    if (!result)
    {
        return false;
    }

    vertexShaderBuffer.Reset();

    result = direct3d->createPixelShader(pixelShader, filename);
    if (!result)
    {
        return false;
    }

    setInitialized();
    return true;
}

bool Shader::render()
{
    bool result = direct3d->setInputLayoutToInputAssembler(inputLayout->getInputLayout());
    if (!result)
    {
        return false;
    }

    result = direct3d->setVertexBufferToInputAssembler(vertexBuffer->getBuffer(),
                                                       vertexBuffer->getStride());
    if (!result)
    {
        return false;
    }

    result = direct3d->setIndexBufferToInputAssembler(indexBuffer->getBuffer());
    if (!result)
    {
        return false;
    }

    for (auto& pair : vertexShaderConstantBuffers)
    {
        result = direct3d->setConstantBufferToVertexShader(pair.second->getBuffer(), pair.first);
        if (!result)
        {
            return false;
        }
    }

    result = direct3d->setVertexShader(vertexShader);
    if (!result)
    {
        return false;
    }

    for (auto& pair : pixelShaderConstantBuffers)
    {
        result = direct3d->setConstantBufferToPixelShader(pair.second->getBuffer(), pair.first);
        if (!result)
        {
            return false;
        }
    }

    for (auto& pair : pixelShaderSamplers)
    {
        result = direct3d->setSamplerStateToPixelShader(pair.second->getState(), pair.first);
        if (!result)
        {
            return false;
        }
    }

    for (auto& pair : pixelShaderTextures)
    {
        result = direct3d->setShaderResourceViewToPixelShader(
            pair.second->getShaderResourceView(), pair.first);
        if (!result)
        {
            return false;
        }
    }

    result = direct3d->setPixelShader(pixelShader);
    if (!result)
    {
        return false;
    }

    result = direct3d->drawIndexed(indexBuffer->getSize());
    if (!result)
    {
        return false;
    }

    return true;
}

void Shader::release()
{
    if (isReleased())
    {
        return;
    }

    pixelShaderTextures.clear();

    pixelShaderSamplers.clear();

    pixelShaderConstantBuffers.clear();
    vertexShaderConstantBuffers.clear();

    indexBuffer.reset();
    vertexBuffer.reset();

    inputLayout.reset();

    pixelShader.Reset();
    vertexShader.Reset();

    setReleased();
}

bool Shader::setVertexBuffer(std::shared_ptr<AbstractVertexBuffer> vertexBuffer)
{
    this->vertexBuffer = vertexBuffer;

    return true;
}

bool Shader::setIndexBuffer(std::shared_ptr<IndexBuffer> indexBuffer)
{
    this->indexBuffer = indexBuffer;

    return true;
}

bool Shader::setPixelShaderTexture(std::shared_ptr<Texture> texture,
                                   uint32 slotIndex)
{
    pixelShaderTextures[slotIndex] = texture;

    return true;
}
