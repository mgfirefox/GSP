#include "Texture2D.h"

const std::function<void(Texture2D*)> Texture2D::deleter = [](Texture2D* texture2D) {
    texture2D->release();
    delete texture2D;
    };

Texture2D::Texture2D() {
    initialized = false;
    released = false;
}

Texture2D::~Texture2D() {
    release();
}

bool Texture2D::isInitialized() {
    return initialized;
}

void Texture2D::setInitialized() {
    initialized = true;
    released = false;
}

bool Texture2D::isReleased() {
    return released;
}

void Texture2D::setReleased() {
    initialized = false;
    released = true;
}

Microsoft::WRL::ComPtr<ID3D11Texture2D> Texture2D::getTexture2D() {
    return texture2D;
}

bool Texture2D::initialize(Microsoft::WRL::ComPtr<ID3D11Device> device, Microsoft::WRL::ComPtr<ID3D11DeviceContext> deviceContext, std::string filename, D3D11_TEXTURE2D_DESC textureDesc, bool& shouldGenerateMipmaps, DXGI_FORMAT& format) {
    if (isInitialized()) {
        release();
    }

    ImageData imageData = {};

    ImageFileParser imageFileParser;
    bool result = imageFileParser.parseFile(filename, imageData);
    if (!result) {
        return false;
    }

    result = initialize(device, deviceContext, imageData, textureDesc, shouldGenerateMipmaps);
    if (!result) {
        return false;
    }

    format = imageData.format;

    setInitialized();
    return true;
}

bool Texture2D::initialize(Microsoft::WRL::ComPtr<ID3D11Device> device, Microsoft::WRL::ComPtr<ID3D11DeviceContext> deviceContext, ImageData imageData, D3D11_TEXTURE2D_DESC texture2DDesc, bool& shouldGenerateMipmaps) {
    texture2DDesc.Width = imageData.width;
    texture2DDesc.Height = imageData.height;

    UINT formatSupport = 0;
    device->CheckFormatSupport(imageData.format, &formatSupport);
    if (imageData.mipmapLevels > 1 || !(formatSupport & (D3D11_FORMAT_SUPPORT_MIP_AUTOGEN | D3D11_FORMAT_SUPPORT_RENDER_TARGET))) {
        shouldGenerateMipmaps = false;
        texture2DDesc.MipLevels = imageData.mipmapLevels;
    }
    else {
        shouldGenerateMipmaps = true;
        texture2DDesc.MipLevels = 0;
    }

    texture2DDesc.ArraySize = 1;
    texture2DDesc.Format = imageData.format;

    if (shouldGenerateMipmaps) {
        texture2DDesc.Usage = D3D11_USAGE_DEFAULT;
        texture2DDesc.BindFlags = D3D11_BIND_RENDER_TARGET;
        texture2DDesc.MiscFlags = D3D11_RESOURCE_MISC_GENERATE_MIPS;
    }
    else {
        texture2DDesc.Usage = D3D11_USAGE_IMMUTABLE;
        texture2DDesc.BindFlags = 0;
        texture2DDesc.MiscFlags = 0;
    }
    texture2DDesc.BindFlags |= D3D11_BIND_SHADER_RESOURCE;

    if (shouldGenerateMipmaps) {
        HRESULT result = device->CreateTexture2D(&texture2DDesc, NULL, texture2D.GetAddressOf());
        if (FAILED(result)) {
            return false;
        }

        deviceContext->UpdateSubresource(texture2D.Get(), 0, NULL, imageData.mipmapDataItems[0].data.data(), imageData.mipmapDataItems[0].rowPitch, imageData.mipmapDataItems[0].depthPitch);
    }
    else {
        std::vector<D3D11_SUBRESOURCE_DATA> initialData;
        initialData.reserve(imageData.mipmapLevels);
        for (unsigned int i = 0; i < imageData.mipmapLevels; i++) {
            D3D11_SUBRESOURCE_DATA subresourceData = {};
            subresourceData.pSysMem = (const void*)imageData.mipmapDataItems[i].data.data();
            subresourceData.SysMemPitch = imageData.mipmapDataItems[i].rowPitch;
            subresourceData.SysMemSlicePitch = imageData.mipmapDataItems[i].depthPitch;
            initialData.push_back(subresourceData);
        }

        HRESULT result = device->CreateTexture2D(&texture2DDesc, initialData.data(), texture2D.GetAddressOf());
        if (FAILED(result)) {
            return false;
        }
    }

    setInitialized();
    return true;
}

void Texture2D::release() {
    if (isReleased()) {
        return;
    }

    texture2D.Reset();

    setReleased();
}
