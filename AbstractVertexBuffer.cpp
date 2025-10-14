#include "AbstractVertexBuffer.h"


AbstractVertexBuffer::AbstractVertexBuffer(std::shared_ptr<Direct3d> direct3d) : buffer()
{
    this->direct3d = direct3d;

    size = 0;
}

AbstractVertexBuffer::~AbstractVertexBuffer()
{
    AbstractVertexBuffer::release();

    direct3d.reset();
}

Microsoft::WRL::ComPtr<ID3D11Buffer> AbstractVertexBuffer::getBuffer()
{
    return buffer;
}

uint32 AbstractVertexBuffer::getSize()
{
    return size;
}

void AbstractVertexBuffer::release()
{
    size = 0;
    buffer.Reset();
}
