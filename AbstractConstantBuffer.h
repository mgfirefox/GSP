#pragma once
#include <d3d11.h>

#include <wrl/client.h>
#include <memory>

#include "Direct3d.h"

#include "IntUtility.h"

class AbstractConstantBuffer
{
protected:
    std::shared_ptr<Direct3d> direct3d;

    Microsoft::WRL::ComPtr<ID3D11Buffer> buffer;
    uint32 size;

public:
    AbstractConstantBuffer(std::shared_ptr<Direct3d> direct3d);
    virtual ~AbstractConstantBuffer();

    Microsoft::WRL::ComPtr<ID3D11Buffer> getBuffer();
    virtual uint32 getSize() = 0;
    uint32 getActualSize();

    virtual bool initialize(D3D11_BUFFER_DESC constantBufferDesc) = 0;
    virtual void release();
};
