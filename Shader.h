#pragma once
#include <d3d11.h>
#include <DirectXMath.h>

#include <wrl/client.h>
#include <memory>

#include <unordered_map>

#include "Direct3d.h"

#include "InputLayout.h"

#include "VertexBuffer.h"
#include "IndexBuffer.h"

#include "ConstantBuffer.h"

#include "Sampler.h"

#include "Texture.h"

#include "IntUtility.h"

class Shader
{
    bool initialized;
    bool released;

    std::shared_ptr<Direct3d> direct3d;

    Microsoft::WRL::ComPtr<ID3D11VertexShader> vertexShader;
    Microsoft::WRL::ComPtr<ID3D11PixelShader> pixelShader;

    std::shared_ptr<InputLayout> inputLayout;

    std::shared_ptr<AbstractVertexBuffer> vertexBuffer;
    std::shared_ptr<IndexBuffer> indexBuffer;

    std::unordered_map<uint32, std::shared_ptr<AbstractConstantBuffer>> vertexShaderConstantBuffers;
    std::unordered_map<uint32, std::shared_ptr<AbstractConstantBuffer>> pixelShaderConstantBuffers;

    std::unordered_map<uint32, std::shared_ptr<Sampler>> pixelShaderSamplers;

    std::unordered_map<uint32, std::shared_ptr<Texture>> pixelShaderTextures;

public:
    Shader(std::shared_ptr<Direct3d> direct3d);
    ~Shader();

private:
    bool isInitialized();
    void setInitialized();

    bool isReleased();
    void setReleased();

public:
    bool setVertexShaderConstantBuffer(std::shared_ptr<AbstractConstantBuffer> constantBuffer,
                                       uint32 slotIndex);
    bool setPixelShaderConstantBuffer(std::shared_ptr<AbstractConstantBuffer> constantBuffer,
                                      uint32 slotIndex);
    bool setPixelShaderSampler(std::shared_ptr<Sampler> sampler, uint32 slotIndex);

    bool initialize(std::wstring filename, const D3D11_INPUT_ELEMENT_DESC* inputElementDescs,
                    uint32 inputElementDescCount);
    bool render();
    void release();

    bool setVertexBuffer(std::shared_ptr<AbstractVertexBuffer> vertexBuffer);
    bool setIndexBuffer(std::shared_ptr<IndexBuffer> indexBuffer);
    template <typename T>
    bool setVertexShaderConstantBufferData(const T* data, uint32 slotIndex);
    template <typename T>
    bool setPixelShaderConstantBufferData(const T* data, uint32 slotIndex);
    bool setPixelShaderTexture(std::shared_ptr<Texture> texture,
                               uint32 slotIndex);
};

template <typename T>
bool Shader::setVertexShaderConstantBufferData(const T* data, uint32 slotIndex)
{
    auto pair = vertexShaderConstantBuffers.find(slotIndex);
    if (pair == vertexShaderConstantBuffers.end())
    {
        return false;
    }

    auto vertexShaderConstantBuffer = std::dynamic_pointer_cast<ConstantBuffer<T>>(pair->second);
    if (!vertexShaderConstantBuffer)
    {
        return false;
    }

    bool result = direct3d->setConstantBufferData(vertexShaderConstantBuffer->getBuffer(), data,
                                                  vertexShaderConstantBuffer->getSize());
    if (!result)
    {
        return false;
    }

    return true;
}

template <typename T>
bool Shader::setPixelShaderConstantBufferData(const T* data, uint32 slotIndex)
{
    auto pair = pixelShaderConstantBuffers.find(slotIndex);
    if (pair == pixelShaderConstantBuffers.end())
    {
        return false;
    }

    auto pixelShaderConstantBuffer = std::dynamic_pointer_cast<ConstantBuffer<T>>(pair->second);
    if (!pixelShaderConstantBuffer)
    {
        return false;
    }

    bool result = direct3d->setConstantBufferData(pixelShaderConstantBuffer->getBuffer(), data,
                                                  pixelShaderConstantBuffer->getSize());
    if (!result)
    {
        return false;
    }

    return true;
}
