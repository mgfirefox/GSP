#pragma once
#include <d3d11.h>
#include <d3dcompiler.h>
#include <DirectXMath.h>

#include <wrl/client.h>
#include <memory>

#include <unordered_map>

#include <wchar.h>
#include <fstream>

#include "InputLayout.h"
#include "ConstantBuffer.h"
#include "Sampler.h"
#include "ShaderResource.h"

class Shader {
    bool initialized;
    bool released;

    Microsoft::WRL::ComPtr<ID3D11VertexShader> vertexShader;
    Microsoft::WRL::ComPtr<ID3D11PixelShader> pixelShader;

    std::unique_ptr<InputLayout> inputLayout;
    
    std::unordered_map<UINT, std::shared_ptr<BaseConstantBuffer>> vsConstantBuffers;
    std::unordered_map<UINT, std::shared_ptr<BaseConstantBuffer>> psConstantBuffers;

    std::unordered_map<UINT, std::shared_ptr<Sampler>> psSamplers;

    std::unordered_map<UINT, std::shared_ptr<ShaderResource>> psShaderResources;

public:
    Shader();
    ~Shader();

private:
    bool isInitialized();
    void setInitialized();

    bool isReleased();
    void setReleased();

public:
    template <class T>
    void setVsConstantBuffer(UINT slot, std::shared_ptr<ConstantBuffer<T>> constantBuffer);
    template <class T>
    void setPsConstantBuffer(UINT slot, std::shared_ptr<ConstantBuffer<T>> constantBuffer);

    void setPsSampler(UINT slot, std::shared_ptr<Sampler> sampler);

    void setPsShaderResource(UINT slot, std::shared_ptr<ShaderResource> shaderResource);

    template <class T>
    bool setVsConstantBufferData(Microsoft::WRL::ComPtr<ID3D11DeviceContext> deviceContext, UINT slot, T data);
    template <class T>
    bool setPsConstantBufferData(Microsoft::WRL::ComPtr<ID3D11DeviceContext> deviceContext, UINT slot, T data);

    bool initialize(Microsoft::WRL::ComPtr<ID3D11Device> device, HWND windowHandle, std::wstring filename, const D3D11_INPUT_ELEMENT_DESC* inputElementsDesc, UINT inputElementsQuantity);
    void set(Microsoft::WRL::ComPtr<ID3D11DeviceContext> deviceContext);
    void release();

private:
    void outputShaderErrorMessage(Microsoft::WRL::ComPtr<ID3D10Blob> errorMessage, HWND windowHandle, std::wstring shaderFilename);
};

template<class T>
inline void Shader::setVsConstantBuffer(UINT slot, std::shared_ptr<ConstantBuffer<T>> constantBuffer) {
    /*if (vsConstantBuffers.size() == D3D11_COMMONSHADER_CONSTANT_BUFFER_API_SLOT_COUNT) {
        return -1;
    }*/

    vsConstantBuffers[slot] = constantBuffer;
}

template<class T>
inline void Shader::setPsConstantBuffer(UINT slot, std::shared_ptr<ConstantBuffer<T>> constantBuffer) {
    psConstantBuffers[slot] = constantBuffer;
}

template<class T>
inline bool Shader::setVsConstantBufferData(Microsoft::WRL::ComPtr<ID3D11DeviceContext> deviceContext, UINT slot, T data) {
    if (vsConstantBuffers.find(slot) == vsConstantBuffers.end()) {
        return false;
    }

    bool result = std::dynamic_pointer_cast<ConstantBuffer<T>>(vsConstantBuffers[slot])->setData(deviceContext, data);
    if (!result) {
        return false;
    }

    return true;
}

template<class T>
inline bool Shader::setPsConstantBufferData(Microsoft::WRL::ComPtr<ID3D11DeviceContext> deviceContext, UINT slot, T data) {
    if (psConstantBuffers.find(slot) == psConstantBuffers.end()) {
        return false;
    }

    bool result = std::dynamic_pointer_cast<ConstantBuffer<T>>(psConstantBuffers[slot])->setData(deviceContext, data);
    if (!result) {
        return false;
    }

    return true;
}
