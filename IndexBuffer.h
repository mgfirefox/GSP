#pragma once
#include <d3d11.h>

#include <wrl/client.h>
#include <memory>

#include "Direct3d.h"

#include "IntUtility.h"

class IndexBuffer
{
    bool initialized;
    bool released;

    std::shared_ptr<Direct3d> direct3d;

    Microsoft::WRL::ComPtr<ID3D11Buffer> buffer;
    uint32 size;

public:
    IndexBuffer(std::shared_ptr<Direct3d> direct3d);
    ~IndexBuffer();

private:
    bool isInitialized();
    void setInitialized();

    bool isReleased();
    void setReleased();

public:
    Microsoft::WRL::ComPtr<ID3D11Buffer> getBuffer();
    uint32 getSize();

    bool initialize(const uint32* indexes,
                    uint32 indexCount);
    void release();
};
