#pragma once
#include <d3d11.h>

#include <wrl/client.h>
#include <memory>

#include "Direct3d.h"

#include "IntUtility.h"

class AbstractVertexBuffer
{
protected:
    std::shared_ptr<Direct3d> direct3d;

    Microsoft::WRL::ComPtr<ID3D11Buffer> buffer;
    uint32 size;

public:
    AbstractVertexBuffer(std::shared_ptr<Direct3d> direct3d);
    virtual ~AbstractVertexBuffer();

    Microsoft::WRL::ComPtr<ID3D11Buffer> getBuffer();
    uint32 getSize();
    virtual uint32 getStride() = 0;

    virtual void release();
};
