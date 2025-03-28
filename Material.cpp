#include "Material.h"

const std::function<void(Material*)> Material::deleter = [](Material* material) {
    material->release();
    delete material;
    };

Material::Material() : diffuseColor{}, diffuseColorTextureResource() {
    initialized = false;
    released = false;

    opacity = 0.0f;

    hasDiffuseColorTexture = false;
}

Material::~Material() {
    release();
}

bool Material::isInitialized() {
    return initialized;
}

void Material::setInitialized() {
    initialized = true;
    released = false;
}

bool Material::isReleased() {
    return released;
}

void Material::setReleased() {
    initialized = false;
    released = true;
}

bool Material::initialize(Microsoft::WRL::ComPtr<ID3D11Device> device, Microsoft::WRL::ComPtr<ID3D11DeviceContext> deviceContext, MaterialData materialData) {
    if (isInitialized()) {
        release();
    }

    diffuseColor = materialData.diffuseColor;
    opacity = materialData.opacity;

    hasDiffuseColorTexture = materialData.hasDiffuseColorImage;
    if (hasDiffuseColorTexture) {
        D3D11_TEXTURE2D_DESC texture2DDesc = {};

        DXGI_SAMPLE_DESC& sampleDesc = texture2DDesc.SampleDesc;
        sampleDesc.Count = 1;
        sampleDesc.Quality = 0;

        bool shouldGenerateMipmaps = false;

        std::shared_ptr<Texture2D> diffuseColorTexture(new Texture2D, Texture2D::deleter);
        bool result = diffuseColorTexture->initialize(device, deviceContext, materialData.diffuseColorImageData, texture2DDesc, shouldGenerateMipmaps);
        if (!result) {
            return false;
        }

        D3D11_SHADER_RESOURCE_VIEW_DESC diffuseColorTextureResourceDesc = {};
        diffuseColorTextureResourceDesc.Format = materialData.diffuseColorImageData.format;
        diffuseColorTextureResourceDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;

        D3D11_TEX2D_SRV& texture2D = diffuseColorTextureResourceDesc.Texture2D;
        texture2D.MostDetailedMip = 0;
        texture2D.MipLevels = -1;

        diffuseColorTextureResource = std::shared_ptr<ShaderResource>(new ShaderResource, ShaderResource::deleter);
        result = diffuseColorTextureResource->initialize(device, deviceContext, diffuseColorTextureResourceDesc, diffuseColorTexture, shouldGenerateMipmaps);
        if (!result) {
            return false;
        }
    }

    setInitialized();
    return true;
}

bool Material::set(Microsoft::WRL::ComPtr<ID3D11DeviceContext> deviceContext, std::unique_ptr<Shader>& shader) {
    MaterialBuffer materialBuffer = {};
    materialBuffer.diffuseColor = diffuseColor;
    materialBuffer.opacity = opacity;
    materialBuffer.hasDiffuseColorTexture = hasDiffuseColorTexture;
    
    bool result = shader->setPsConstantBufferData(deviceContext, PS_MATERIAL_BUFFER_SLOT, materialBuffer);
    if (!result) {
        return false;
    }

    if (hasDiffuseColorTexture) {
        shader->setPsShaderResource(PS_DIFFUSE_COLOR_TEXTURE_RESOURCE_SLOT, diffuseColorTextureResource);
    }

    return true;
}

void Material::release() {
    if (isReleased()) {
        return;
    }

    hasDiffuseColorTexture = false;
    diffuseColorTextureResource.reset();

    opacity = 0.0f;
    diffuseColor = {};

    setReleased();
}
