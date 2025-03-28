#pragma once
#include <d3d11.h>

#include <wrl/client.h>
#include <memory>

#include <functional>

#include "Texture2D.h"

#include "Shader.h"
#include "ShaderResource.h"

#include "FileParsers.h"

const UINT PS_MATERIAL_BUFFER_SLOT = 0;

const UINT PS_DIFFUSE_COLOR_TEXTURE_RESOURCE_SLOT = 0;

class Material {
    bool initialized;
    bool released;

    DirectX::XMFLOAT3 diffuseColor;
    float opacity;

    bool hasDiffuseColorTexture;
    std::shared_ptr<ShaderResource> diffuseColorTextureResource;

public:
    static const std::function<void(Material*)> deleter;

    Material();
    ~Material();

private:
    bool isInitialized();
    void setInitialized();

    bool isReleased();
    void setReleased();

public:
    bool initialize(Microsoft::WRL::ComPtr<ID3D11Device> device, Microsoft::WRL::ComPtr<ID3D11DeviceContext> deviceContext, MaterialData materialData);
    bool set(Microsoft::WRL::ComPtr<ID3D11DeviceContext> deviceContext, std::unique_ptr<Shader>& shader);
    void release();
};