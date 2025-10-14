#include "Texture.h"

Texture::Texture(std::shared_ptr<Direct3d> direct3d) : fileParser(), buffer(), shaderResourceView()
{
    initialized = false;
    released = false;

    this->direct3d = direct3d;
}

Texture::~Texture()
{
    release();

    direct3d.reset();
}

bool Texture::isInitialized()
{
    return initialized;
}

void Texture::setInitialized()
{
    initialized = true;
    released = false;
}

bool Texture::isReleased()
{
    return released;
}

void Texture::setReleased()
{
    initialized = false;
    released = true;
}

Microsoft::WRL::ComPtr<ID3D11Texture2D> Texture::getBuffer()
{
    return buffer;
}

Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> Texture::getShaderResourceView()
{
    return shaderResourceView;
}

bool Texture::initialize(std::string filename)
{
    if (isInitialized())
    {
        release();
    }

    ImageData imageData = {};

    bool result = readImageData(filename, imageData);
    if (!result)
    {
        return false;
    }

    bool shouldGenerateMipmaps = false;

    result = initializeBuffer(imageData, shouldGenerateMipmaps);
    if (!result)
    {
        return false;
    }

    result = initializeShaderResourceView(imageData.format, shouldGenerateMipmaps);
    if (!result)
    {
        return false;
    }

    setInitialized();
    return true;
}

bool Texture::initialize(ImageData imageData)
{
    if (isInitialized())
    {
        release();
    }

    bool shouldGenerateMipmaps = false;

    bool result = initializeBuffer(imageData, shouldGenerateMipmaps);
    if (!result)
    {
        return false;
    }

    result = initializeShaderResourceView(imageData.format, shouldGenerateMipmaps);
    if (!result)
    {
        return false;
    }

    setInitialized();
    return true;
}

void Texture::release()
{
    if (isReleased())
    {
        return;
    }

    shaderResourceView.Reset();

    buffer.Reset();

    setReleased();
}

bool Texture::readImageData(std::string filename, ImageData& imageData)
{
    bool result = fileParser.parseFile(filename, imageData);
    if (!result)
    {
        return false;
    }

    return true;
}

bool Texture::initializeBuffer(ImageData imageData, bool& shouldGenerateMipmaps)
{
    D3D11_TEXTURE2D_DESC texture2dDesc = {};

    texture2dDesc.Width = imageData.width;
    texture2dDesc.Height = imageData.height;
    texture2dDesc.ArraySize = 1;
    texture2dDesc.Format = imageData.format;

    DXGI_SAMPLE_DESC& sampleDesc = texture2dDesc.SampleDesc;
    sampleDesc.Count = 1;
    sampleDesc.Quality = 0;

    std::vector<D3D11_SUBRESOURCE_DATA> initialData;

    if (imageData.mipmapLevels < 2 && direct3d->canMipmapBeGenerated(imageData.format))
    {
        shouldGenerateMipmaps = true;

        texture2dDesc.MipLevels = 0;

        texture2dDesc.Usage = D3D11_USAGE_DEFAULT;
        texture2dDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET;
        texture2dDesc.MiscFlags = D3D11_RESOURCE_MISC_GENERATE_MIPS;

        MipmapData mipmapData = imageData.mipmapDataItems[0];

        D3D11_SUBRESOURCE_DATA subresourceData = {};
        subresourceData.pSysMem = mipmapData.data.data();
        subresourceData.SysMemPitch = mipmapData.rowPitch;
        subresourceData.SysMemSlicePitch = mipmapData.depthPitch;

        initialData = {subresourceData};
    }
    else
    {
        shouldGenerateMipmaps = false;

        texture2dDesc.MipLevels = imageData.mipmapLevels;

        texture2dDesc.Usage = D3D11_USAGE_IMMUTABLE;
        texture2dDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
        texture2dDesc.MiscFlags = 0;

        initialData = std::vector<D3D11_SUBRESOURCE_DATA>(imageData.mipmapLevels);
        for (uint32 i = 0; i < imageData.mipmapLevels; i++)
        {
            MipmapData& mipmapData = imageData.mipmapDataItems[i];
            D3D11_SUBRESOURCE_DATA& subresourceData = initialData[i];

            subresourceData.pSysMem = mipmapData.data.data();
            subresourceData.SysMemPitch = mipmapData.rowPitch;
            subresourceData.SysMemSlicePitch = mipmapData.depthPitch;
        }
    }

    bool result = direct3d->createTexture2d(buffer, texture2dDesc, initialData.data());
    if (!result)
    {
        return false;
    }

    return true;
}

bool Texture::initializeShaderResourceView(DXGI_FORMAT format, bool shouldGenerateMipmaps)
{
    D3D11_SHADER_RESOURCE_VIEW_DESC shaderResourceViewDesc = {};
    shaderResourceViewDesc.Format = format;
    shaderResourceViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;

    D3D11_TEX2D_SRV& texture2d = shaderResourceViewDesc.Texture2D;
    texture2d.MostDetailedMip = 0;
    texture2d.MipLevels = -1;

    bool result = direct3d->createShaderResourceView(shaderResourceView, shaderResourceViewDesc,
                                                     buffer, shouldGenerateMipmaps);
    if (!result)
    {
        return false;
    }

    return true;
}
