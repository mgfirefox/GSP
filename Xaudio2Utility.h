#pragma once

template <typename T>
struct Xaudio2Deleter
{
    void operator()(T* pointer);
};

template <typename T>
void Xaudio2Deleter<T>::operator()(T* pointer)
{
    pointer->DestroyVoice();
}
