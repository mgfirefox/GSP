#include "AbstractConstantBuffer.h"

AbstractConstantBuffer::AbstractConstantBuffer(std::shared_ptr<Direct3d> direct3d) : buffer()
{
    this->direct3d = direct3d;

    size = 0;
}

AbstractConstantBuffer::~AbstractConstantBuffer()
{
    AbstractConstantBuffer::release();

    direct3d.reset();
}

Microsoft::WRL::ComPtr<ID3D11Buffer> AbstractConstantBuffer::getBuffer()
{
    return buffer;
}

uint32 AbstractConstantBuffer::getActualSize()
{
    return size;
}

void AbstractConstantBuffer::release()
{
    size = 0;
    buffer.Reset();
}
