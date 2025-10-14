#pragma once
#include <memory>

template <typename T>
struct Deleter
{
    void operator()(T* pointer);
};

template <typename T, typename TDeleter = Deleter<T>, typename... Types>
std::shared_ptr<T> createSharedPointer(Types&&... params)
{
    return std::shared_ptr<T>(new T(std::forward<Types>(params)...), TDeleter());
}

template <typename T, typename TDeleter = Deleter<T>>
std::shared_ptr<T> copyFromSharedPointer(std::shared_ptr<T> pointer)
{
    return createSharedPointer<T, TDeleter>(*pointer);
}

template <typename T, typename TDeleter = Deleter<T>, typename... Types>
std::unique_ptr<T, TDeleter> createUniquePointer(Types&&... params)
{
    return std::unique_ptr<T, TDeleter>(new T(std::forward<Types>(params)...), TDeleter());
}

template <typename T>
void Deleter<T>::operator()(T* pointer)
{
    pointer->release();
    delete pointer;
}
