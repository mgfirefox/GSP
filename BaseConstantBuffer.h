#pragma once
#include <d3d11.h>

#include <wrl/client.h>

#include <functional>

class BaseConstantBuffer {
protected:
    UINT size;

    Microsoft::WRL::ComPtr<ID3D11Buffer> constantBuffer;

public:
    static const std::function<void(BaseConstantBuffer*)> deleter;

    BaseConstantBuffer();
    virtual ~BaseConstantBuffer();

    virtual Microsoft::WRL::ComPtr<ID3D11Buffer> getConstantBuffer() = 0;

    virtual bool initialize(Microsoft::WRL::ComPtr<ID3D11Device> device, D3D11_BUFFER_DESC constantBufferDesc) = 0;
    virtual void setVs(Microsoft::WRL::ComPtr<ID3D11DeviceContext> deviceContext, UINT slot) = 0;
    virtual void setPs(Microsoft::WRL::ComPtr<ID3D11DeviceContext> deviceContext, UINT slot) = 0;
    virtual void release();
};

