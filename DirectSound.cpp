#include "DirectSound.h"

DirectSound::DirectSound(std::shared_ptr<Window> window) : waveFormat{}, listener3dPosition{}
{
    initialized = false;
    released = false;

    this->window = window;
}

DirectSound::~DirectSound()
{
    release();

    window.reset();
}

bool DirectSound::isInitialized()
{
    return initialized;
}

void DirectSound::setInitialized()
{
    initialized = true;
    released = false;
}

bool DirectSound::isReleased()
{
    return released;
}

void DirectSound::setReleased()
{
    initialized = false;
    released = true;
}

Microsoft::WRL::ComPtr<IDirectSound8> DirectSound::getDirectSound()
{
    return directSound;
}

Microsoft::WRL::ComPtr<IDirectSoundBuffer> DirectSound::getPrimaryBuffer()
{
    return primaryBuffer;
}

Microsoft::WRL::ComPtr<IDirectSound3DListener> DirectSound::getListener3d()
{
    return listener3d;
}

DirectX::XMFLOAT3 DirectSound::getListener3dPosition()
{
    return listener3dPosition;
}

bool DirectSound::setListener3dPosition(DirectX::XMFLOAT3 position)
{
    HRESULT result = listener3d->SetPosition(position.x, position.y, position.z, DS3D_IMMEDIATE);
    if (FAILED(result))
    {
        return false;
    }

    this->listener3dPosition = position;

    return true;
}

bool DirectSound::setListener3dOrientation(DirectX::XMFLOAT3 forward, DirectX::XMFLOAT3 up)
{
    HRESULT result = listener3d->SetOrientation(forward.x, forward.y, forward.z, up.x, up.y, up.z,
                                                DS3D_IMMEDIATE);
    if (FAILED(result))
    {
        return false;
    }

    return true;
}

bool DirectSound::initialize(WAVEFORMATEX waveFormat)
{
    if (isInitialized())
    {
        release();
    }

    bool result = initializeDirectSound8();
    if (!result)
    {
        return false;
    }

    this->waveFormat = waveFormat;

    result = initializePrimaryBuffer();
    if (!result)
    {
        return false;
    }

    result = initializeListener3d8();
    if (!result)
    {
        return false;
    }

    setInitialized();
    return true;
}

void DirectSound::release()
{
    if (isReleased())
    {
        return;
    }

    listener3dPosition = {};

    waveFormat = {};

    listener3d.Reset();

    primaryBuffer.Reset();

    directSound.Reset();

    setReleased();
}

bool DirectSound::createSecondaryBuffer(Microsoft::WRL::ComPtr<IDirectSoundBuffer>& secondaryBuffer,
                                        DSBUFFERDESC secondaryBufferDesc, bool is3d)
{
    if (is3d)
    {
        secondaryBufferDesc.dwFlags |= DSBCAPS_CTRL3D;
    }

    WAVEFORMATEX* waveFormat = secondaryBufferDesc.lpwfxFormat;
    if (waveFormat->wFormatTag != this->waveFormat.wFormatTag)
    {
        return false;
    }
    if (waveFormat->nChannels != this->waveFormat.nChannels)
    {
        if (is3d)
        {
            if (waveFormat->nChannels != 1)
            {
                return false;
            }
        }
    }
    if (waveFormat->nSamplesPerSec != this->waveFormat.nSamplesPerSec)
    {
        return false;
    }
    if (waveFormat->wBitsPerSample != this->waveFormat.wBitsPerSample)
    {
        return false;
    }

    if (!is3d)
    {
        waveFormat->nAvgBytesPerSec = this->waveFormat.nAvgBytesPerSec;
        waveFormat->nBlockAlign = this->waveFormat.nBlockAlign;
    }

    HRESULT result = directSound->CreateSoundBuffer(
        &secondaryBufferDesc, secondaryBuffer.GetAddressOf(), nullptr);
    if (FAILED(result))
    {
        return false;
    }

    return true;
}

bool DirectSound::createSecondaryBuffer8(
    Microsoft::WRL::ComPtr<IDirectSoundBuffer8>& secondaryBuffer, DSBUFFERDESC secondaryBufferDesc, bool
    is3d)
{
    Microsoft::WRL::ComPtr<IDirectSoundBuffer> tempSecondaryBuffer;

    bool result = createSecondaryBuffer(tempSecondaryBuffer, secondaryBufferDesc, is3d);
    if (!result)
    {
        return false;
    }

    HRESULT hresult = tempSecondaryBuffer->QueryInterface(IID_IDirectSoundBuffer8,
                                                          reinterpret_cast<void**>(secondaryBuffer.
                                                              GetAddressOf()));
    if (FAILED(hresult))
    {
        return false;
    }

    tempSecondaryBuffer.Reset();

    return true;
}

bool DirectSound::initializeDirectSound8()
{
    HRESULT result = DirectSoundCreate8(nullptr, directSound.GetAddressOf(), nullptr);
    if (FAILED(result))
    {
        return false;
    }

    result = directSound->SetCooperativeLevel(window->getHandle(), DSSCL_PRIORITY);
    if (FAILED(result))
    {
        return false;
    }

    return true;
}

bool DirectSound::initializePrimaryBuffer()
{
    DSBUFFERDESC primaryBufferDesc = {};
    primaryBufferDesc.dwSize = sizeof(DSBUFFERDESC);
    primaryBufferDesc.dwFlags = DSBCAPS_CTRL3D | DSBCAPS_CTRLVOLUME | DSBCAPS_PRIMARYBUFFER;
    primaryBufferDesc.dwBufferBytes = 0;
    primaryBufferDesc.dwReserved = 0;
    primaryBufferDesc.lpwfxFormat = nullptr;
    primaryBufferDesc.guid3DAlgorithm = DS3DALG_DEFAULT;

    HRESULT result = directSound->CreateSoundBuffer(&primaryBufferDesc,
                                                    primaryBuffer.GetAddressOf(), nullptr);
    if (FAILED(result))
    {
        return false;
    }

    WAVEFORMATEX defaultWaveFormat = {};

    result = primaryBuffer->GetFormat(&defaultWaveFormat, sizeof(WAVEFORMATEX), nullptr);
    if (FAILED(result))
    {
        return false;
    }

    if (waveFormat.wFormatTag == 0)
    {
        waveFormat.wFormatTag = defaultWaveFormat.wFormatTag;
    }
    if (waveFormat.nChannels == 0)
    {
        waveFormat.nChannels = defaultWaveFormat.nChannels;
    }
    if (waveFormat.nSamplesPerSec == 0)
    {
        waveFormat.nSamplesPerSec = defaultWaveFormat.nSamplesPerSec;
    }
    if (waveFormat.wBitsPerSample == 0)
    {
        waveFormat.wBitsPerSample = defaultWaveFormat.wBitsPerSample;
    }
    waveFormat.nBlockAlign = waveFormat.wBitsPerSample / 8 * waveFormat.nChannels;
    waveFormat.nAvgBytesPerSec = waveFormat.nSamplesPerSec * waveFormat.nBlockAlign;

    result = primaryBuffer->SetFormat(&waveFormat);
    if (FAILED(result))
    {
        return false;
    }

    return true;
}

bool DirectSound::initializeListener3d8()
{
    HRESULT result = primaryBuffer->QueryInterface(
        IID_IDirectSound3DListener8, reinterpret_cast<void**>(listener3d.GetAddressOf()));
    if (FAILED(result))
    {
        return false;
    }

    return true;
}
