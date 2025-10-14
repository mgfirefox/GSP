#include "Material.h"

Material::Material(std::shared_ptr<Shader> shader,
                   std::shared_ptr<Direct3d> direct3d) : diffuseColor{}, diffuseColorTexture()
{
    initialized = false;
    released = false;

    this->shader = shader;

    this->direct3d = direct3d;

    opacity = 0.0f;

    hasDiffuseColorTexture = false;
}

Material::Material(const Material& material)
{
    initialized = material.initialized;
    released = material.released;

    this->shader = material.shader;
    this->direct3d = material.direct3d;

    diffuseColor = material.diffuseColor;
    opacity = material.opacity;

    hasDiffuseColorTexture = material.hasDiffuseColorTexture;
    diffuseColorTexture = material.diffuseColorTexture;
}

Material::~Material()
{
    release();

    shader.reset();

    direct3d.reset();
}

bool Material::isInitialized()
{
    return initialized;
}

void Material::setInitialized()
{
    initialized = true;
    released = false;
}

bool Material::isReleased()
{
    return released;
}

void Material::setReleased()
{
    initialized = false;
    released = true;
}

bool Material::initialize(MaterialData materialData)
{
    if (isInitialized())
    {
        release();
    }

    bool result = initializeData(materialData);
    if (!result)
    {
        return false;
    }

    setInitialized();
    return true;
}

bool Material::render()
{
    MaterialBuffer materialBuffer = {};
    materialBuffer.diffuseColor = diffuseColor;
    materialBuffer.opacity = opacity;
    materialBuffer.hasDiffuseColorTexture = hasDiffuseColorTexture;

    bool result = shader->setPixelShaderConstantBufferData(&materialBuffer,
                                                           PsMaterialBufferSlotIndex);
    if (!result)
    {
        return false;
    }

    if (hasDiffuseColorTexture)
    {
        result = shader->setPixelShaderTexture(diffuseColorTexture,
                                               PsDiffuseColorTextureResourceSlotIndex);
        if (!result)
        {
            return false;
        }
    }

    return true;
}

void Material::release()
{
    if (isReleased())
    {
        return;
    }

    hasDiffuseColorTexture = false;
    diffuseColorTexture.reset();

    opacity = 0.0f;
    diffuseColor = {};

    setReleased();
}

bool Material::initializeData(MaterialData materialData)
{
    diffuseColor = materialData.diffuseColor;
    opacity = materialData.opacity;

    hasDiffuseColorTexture = materialData.hasDiffuseColorImage;
    if (hasDiffuseColorTexture)
    {
        diffuseColorTexture = createSharedPointer<Texture>(direct3d);
        bool result = diffuseColorTexture->initialize(materialData.diffuseColorImageData);
        if (!result)
        {
            return false;
        }
    }

    return true;
}
