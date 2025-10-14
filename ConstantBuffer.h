#pragma once
#include "AbstractConstantBuffer.h"

template <typename T>
class ConstantBuffer : public AbstractConstantBuffer
{
    bool initialized;
    bool released;

public:
    ConstantBuffer(std::shared_ptr<Direct3d> direct3d);
    ~ConstantBuffer() override;

private:
    bool isInitialized();
    void setInitialized();

    bool isReleased();
    void setReleased();

public:
    uint32 getSize() override;

    bool initialize(D3D11_BUFFER_DESC constantBufferDesc) override;
    void release() override;
};

template <typename T>
ConstantBuffer<
    T>::ConstantBuffer(std::shared_ptr<Direct3d> direct3d) : AbstractConstantBuffer(direct3d)
{
    initialized = false;
    released = false;
}

template <typename T>
ConstantBuffer<T>::~ConstantBuffer()
{
    ConstantBuffer<T>::release();
}

template <typename T>
bool ConstantBuffer<T>::isInitialized()
{
    return initialized;
}

template <typename T>
void ConstantBuffer<T>::setInitialized()
{
    initialized = true;
    released = false;
}

template <typename T>
bool ConstantBuffer<T>::isReleased()
{
    return released;
}

template <typename T>
void ConstantBuffer<T>::setReleased()
{
    initialized = false;
    released = true;
}

template <typename T>
uint32 ConstantBuffer<T>::getSize()
{
    return sizeof(T);
}

template <typename T>
bool ConstantBuffer<T>::initialize(D3D11_BUFFER_DESC constantBufferDesc)
{
    if (isInitialized())
    {
        release();
    }

    bool result = direct3d->createConstantBuffer(buffer, constantBufferDesc, size);
    if (!result)
    {
        return false;
    }

    setInitialized();
    return true;
}

template <typename T>
void ConstantBuffer<T>::release()
{
    if (isReleased())
    {
        return;
    }

    AbstractConstantBuffer::release();

    setReleased();
}
