#pragma once
#include "AbstractVertexBuffer.h"

template <typename T>
class VertexBuffer : public AbstractVertexBuffer
{
    bool initialized;
    bool released;

public:
    VertexBuffer(std::shared_ptr<Direct3d> direct3d);
    ~VertexBuffer() override;

private:
    bool isInitialized();
    void setInitialized();

    bool isReleased();
    void setReleased();

public:
    uint32 getStride() override;

    bool initialize(const T* vertexes, uint32 vertexCount);
    void release() override;
};

template <typename T>
VertexBuffer<T>::VertexBuffer(std::shared_ptr<Direct3d> direct3d) : AbstractVertexBuffer(direct3d)
{
    initialized = false;
    released = false;
}

template <typename T>
VertexBuffer<T>::~VertexBuffer()
{
    release();
}

template <typename T>
bool VertexBuffer<T>::isInitialized()
{
    return initialized;
}

template <typename T>
void VertexBuffer<T>::setInitialized()
{
    initialized = true;
    released = false;
}

template <typename T>
bool VertexBuffer<T>::isReleased()
{
    return released;
}

template <typename T>
void VertexBuffer<T>::setReleased()
{
    initialized = false;
    released = true;
}

template <typename T>
uint32 VertexBuffer<T>::getStride()
{
    return sizeof(T);
}

template <typename T>
bool VertexBuffer<T>::initialize(const T* vertexes,
                                 uint32 vertexCount)
{
    if (isInitialized())
    {
        release();
    }

    bool result = direct3d->createVertexBuffer(buffer, vertexes, sizeof(T), vertexCount);
    if (!result)
    {
        return false;
    }

    size = vertexCount;

    setInitialized();
    return true;
}

template <typename T>
void VertexBuffer<T>::release()
{
    if (isReleased())
    {
        return;
    }

    AbstractVertexBuffer::release();

    setReleased();
}
