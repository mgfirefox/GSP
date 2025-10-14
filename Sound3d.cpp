#include "Sound3d.h"

Sound3d::Sound3d(std::shared_ptr<DirectSound> directSound) : Sound(directSound),
                                                             secondaryBuffer3d(), position{}
{
    initialized = false;
    released = false;

    maxDistance = 0.0f;
}

Sound3d::~Sound3d()
{
    Sound3d::release();
}

bool Sound3d::isInitialized()
{
    return initialized;
}

void Sound3d::setInitialized()
{
    initialized = true;
    released = false;
}

bool Sound3d::isReleased()
{
    return released;
}

void Sound3d::setReleased()
{
    initialized = false;
    released = true;
}

Microsoft::WRL::ComPtr<IDirectSound3DBuffer> Sound3d::getSecondaryBuffer3d()
{
    return secondaryBuffer3d;
}

bool Sound3d::setMinDistance(float minDistance)
{
    HRESULT result = secondaryBuffer3d->SetMinDistance(minDistance, DS3D_IMMEDIATE);
    if (FAILED(result))
    {
        return false;
    }

    return true;
}

float Sound3d::getMaxDistance()
{
    return maxDistance;
}

bool Sound3d::setMaxDistance(float maxDistance)
{
    HRESULT result = secondaryBuffer3d->SetMaxDistance(maxDistance, DS3D_IMMEDIATE);
    if (FAILED(result))
    {
        return false;
    }

    this->maxDistance = maxDistance;

    return true;
}

DirectX::XMFLOAT3 Sound3d::getPosition()
{
    return position;
}

bool Sound3d::setPosition(DirectX::XMFLOAT3 position)
{
    HRESULT result = secondaryBuffer3d->SetPosition(position.x, position.y, position.z,
                                                    DS3D_IMMEDIATE);
    if (FAILED(result))
    {
        return false;
    }

    this->position = position;

    return true;
}

bool Sound3d::initialize(std::string filename, bool isLooping, float minDistance, float maxDistance,
                         int32 volume, DirectX::XMFLOAT3 position)
{
    if (isInitialized())
    {
        release();
    }

    bool result = Sound::initialize(filename, isLooping, volume, true);
    if (!result)
    {
        return false;
    }

    result = initializeSecondaryBuffer3d8();
    if (!result)
    {
        return false;
    }

    result = setMinDistance(minDistance);
    if (!result)
    {
        return false;
    }

    result = setMaxDistance(maxDistance);
    if (!result)
    {
        return false;
    }

    result = setPosition(position);
    if (!result)
    {
        return false;
    }

    setInitialized();
    return true;
}

bool Sound3d::initialize(SoundData soundData, bool isLooping, float minDistance, float maxDistance,
                         int32 volume, DirectX::XMFLOAT3 position)
{
    if (isInitialized())
    {
        release();
    }

    bool result = Sound::initialize(soundData, isLooping, volume, true);
    if (!result)
    {
        return false;
    }

    result = initializeSecondaryBuffer3d8();
    if (!result)
    {
        return false;
    }

    result = setMinDistance(minDistance);
    if (!result)
    {
        return false;
    }

    result = setMaxDistance(maxDistance);
    if (!result)
    {
        return false;
    }

    result = setPosition(position);
    if (!result)
    {
        return false;
    }

    setInitialized();
    return true;
}

void Sound3d::release()
{
    if (isReleased())
    {
        return;
    }

    maxDistance = 0.0f;

    secondaryBuffer3d.Reset();

    Sound::release();

    setReleased();
}

bool Sound3d::initializeSecondaryBuffer8(SoundData soundData, bool is3d)
{
    if (soundData.numChannels != 1)
    {
        return false;
    }

    bool result = Sound::initializeSecondaryBuffer8(soundData, is3d);
    if (!result)
    {
        return false;
    }

    return true;
}

bool Sound3d::initializeSecondaryBuffer3d8()
{
    HRESULT result = secondaryBuffer->QueryInterface(
        IID_IDirectSound3DBuffer8, reinterpret_cast<void**>(secondaryBuffer3d.GetAddressOf()));
    if (FAILED(result))
    {
        return false;
    }

    return true;
}
