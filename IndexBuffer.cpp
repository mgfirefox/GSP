#include "IndexBuffer.h"

IndexBuffer::IndexBuffer(std::shared_ptr<Direct3d> direct3d) : buffer()
{
    initialized = false;
    released = false;

    this->direct3d = direct3d;

    size = 0;
}

IndexBuffer::~IndexBuffer()
{
    release();

    direct3d.reset();
}

bool IndexBuffer::isInitialized()
{
    return initialized;
}

void IndexBuffer::setInitialized()
{
    initialized = true;
    released = false;
}

bool IndexBuffer::isReleased()
{
    return released;
}

void IndexBuffer::setReleased()
{
    initialized = false;
    released = true;
}

uint32 IndexBuffer::getSize()
{
    return size;
}

Microsoft::WRL::ComPtr<ID3D11Buffer> IndexBuffer::getBuffer()
{
    return buffer;
}

bool IndexBuffer::initialize(
    const uint32* indexes, uint32 indexCount)
{
    if (isInitialized())
    {
        release();
    }

    bool result = direct3d->createIndexBuffer(buffer, indexes, indexCount);
    if (!result)
    {
        return false;
    }

    size = indexCount;

    setInitialized();
    return true;
}

void IndexBuffer::release()
{
    if (isReleased())
    {
        return;
    }

    size = 0;
    buffer.Reset();

    setReleased();
}
