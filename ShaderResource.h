#pragma once
#include <d3d11.h>

#include <wrl/client.h>
#include <memory>

#include <functional>

#include "Texture2D.h"

class ShaderResource {
    bool initialized;
    bool released;

    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> shaderResourceView;

public:
    static const std::function<void(ShaderResource*)> deleter;

    ShaderResource();
    ~ShaderResource();

private:
    bool isInitialized();
    void setInitialized();

    bool isReleased();
    void setReleased();

public:
    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> getShaderResource();

    bool initialize(Microsoft::WRL::ComPtr<ID3D11Device> device, Microsoft::WRL::ComPtr<ID3D11DeviceContext> deviceContext, D3D11_SHADER_RESOURCE_VIEW_DESC shaderResourceViewDesc, std::shared_ptr<Texture2D> texture2D, bool shouldGenerateMipmaps = false);
    void setPs(Microsoft::WRL::ComPtr<ID3D11DeviceContext> deviceContext, UINT slot);
    void release();
};
