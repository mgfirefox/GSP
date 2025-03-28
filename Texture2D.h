#pragma once
#include <d3d11.h>

#include <wrl/client.h>
#include <memory>

#include <functional>

#include <string>

#include "FileParsers.h"

class Texture2D {
    bool initialized;
    bool released;

    Microsoft::WRL::ComPtr<ID3D11Texture2D> texture2D;

public:
    static const std::function<void(Texture2D*)> deleter;

    Texture2D();
    ~Texture2D();

private:
    bool isInitialized();
    void setInitialized();

    bool isReleased();
    void setReleased();

public:
    Microsoft::WRL::ComPtr<ID3D11Texture2D> getTexture2D();

    bool initialize(Microsoft::WRL::ComPtr<ID3D11Device> device, Microsoft::WRL::ComPtr<ID3D11DeviceContext> deviceContext, std::string filename, D3D11_TEXTURE2D_DESC texture2DDesc, bool& shouldGenerateMipmaps, DXGI_FORMAT& format);
    bool initialize(Microsoft::WRL::ComPtr<ID3D11Device> device, Microsoft::WRL::ComPtr<ID3D11DeviceContext> deviceContext, ImageData imageData, D3D11_TEXTURE2D_DESC texture2DDesc, bool& shouldGenerateMipmaps);
    void release();
};