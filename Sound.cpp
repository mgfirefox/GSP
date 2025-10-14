#include "Sound.h"

Sound::Sound(std::shared_ptr<DirectSound> directSound) : fileParser(), secondaryBuffer()
{
    initialized = false;
    released = false;

    this->directSound = directSound;

    volume = 0;

    looping = false;

    state = SoundState::Undefined;

    muted = false;
}

Sound::~Sound()
{
    Sound::release();

    directSound.reset();
}

bool Sound::isInitialized()
{
    return initialized;
}

void Sound::setInitialized()
{
    initialized = true;
    released = false;
}

bool Sound::isReleased()
{
    return released;
}

void Sound::setReleased()
{
    initialized = false;
    released = true;
}

Microsoft::WRL::ComPtr<IDirectSoundBuffer8> Sound::getSecondaryBuffer()
{
    return secondaryBuffer;
}

bool Sound::setVolume(int32 volume)
{
    if (!muted)
    {
        HRESULT result = secondaryBuffer->SetVolume(volume);
        if (FAILED(result))
        {
            return false;
        }
    }

    this->volume = volume;

    return true;
}

bool Sound::isLooping()
{
    return looping;
}

SoundState Sound::getState()
{
    updateState();

    return state;
}

bool Sound::isPlaying()
{
    return getState() == SoundState::Playing;
}

bool Sound::isPaused()
{
    return getState() == SoundState::Paused;
}

bool Sound::isStopped()
{
    return getState() == SoundState::Stopped;
}

bool Sound::isMuted()
{
    return muted;
}

bool Sound::initialize(std::string filename, bool isLooping, int32 volume, bool is3d)
{
    if (isInitialized())
    {
        release();
    }

    SoundData soundData = {};

    bool result = readData(filename, soundData);
    if (!result)
    {
        return false;
    }

    result = initializeSecondaryBuffer8(soundData, is3d);
    if (!result)
    {
        return false;
    }

    looping = isLooping;
    
    result = setVolume(volume);
    if (!result)
    {
        return false;
    }
    
    state = SoundState::Stopped;

    setInitialized();
    return true;
}

bool Sound::initialize(SoundData soundData, bool isLooping, int32 volume, bool is3d)
{
    if (isInitialized())
    {
        release();
    }

    bool result = initializeSecondaryBuffer8(soundData, is3d);
    if (!result)
    {
        return false;
    }

    looping = isLooping;

    result = setVolume(volume);
    if (!result)
    {
        return false;
    }
    
    state = SoundState::Stopped;

    setInitialized();
    return true;
}

void Sound::release()
{
    if (isReleased())
    {
        return;
    }

    if (state != SoundState::Undefined)
    {
        stop();

        state = SoundState::Undefined;
    }

    muted = false;
    
    looping = false;

    volume = 0;

    secondaryBuffer.Reset();

    setReleased();
}

bool Sound::play()
{
    if (!isPlaying())
    {
        int32 flags = 0;
        if (looping)
        {
            flags |= DSBPLAY_LOOPING;
        }

        HRESULT result = secondaryBuffer->Play(0, 0, flags);
        if (FAILED(result))
        {
            return false;
        }
        
        state = SoundState::Playing;
    }

    return true;
}

bool Sound::pause()
{
    SoundState state = getState();
    
    if (state != SoundState::Paused && state != SoundState::Stopped)
    {
        HRESULT result = secondaryBuffer->Stop();
        if (FAILED(result))
        {
            return false;
        }

        this->state = SoundState::Paused;
    }

    return true;
}

bool Sound::stop()
{
    if (!isStopped())
    {
        HRESULT result = secondaryBuffer->Stop();
        if (FAILED(result))
        {
            return false;
        }

        result = secondaryBuffer->SetCurrentPosition(0);
        if (FAILED(result))
        {
            return false;
        }

        state = SoundState::Stopped;
    }

    return true;
}

bool Sound::mute()
{
    if (!isMuted())
    {
        HRESULT result = secondaryBuffer->SetVolume(DSBVOLUME_MIN);
        if (FAILED(result))
        {
            return false;
        }
        
        muted = true;
    }


    return true;
}

bool Sound::unmute()
{
    if (isMuted())
    {
        HRESULT result = secondaryBuffer->SetVolume(volume);
        if (FAILED(result))
        {
            return false;
        }
        
        muted = false;
    }


    return true;
}

void Sound::updateState()
{
    if (state != SoundState::Playing)
    {
        return;
    }

    uint32 status = 0;

    HRESULT result = secondaryBuffer->GetStatus(reinterpret_cast<LPDWORD>(&status));
    if (FAILED(result))
    {
        return;
    }

    if (status & DSBSTATUS_PLAYING)
    {
        return;
    }

    state = SoundState::Stopped;
}

bool Sound::readData(std::string filename, SoundData& soundData)
{
    bool result = fileParser.parseFile(filename, soundData);
    if (!result)
    {
        return false;
    }

    return true;
}

bool Sound::initializeSecondaryBuffer8(SoundData soundData, bool is3d)
{
    WAVEFORMATEX waveFormat = {};
    waveFormat.wFormatTag = soundData.format;
    waveFormat.nChannels = soundData.numChannels;
    waveFormat.nSamplesPerSec = soundData.sampleRate;
    waveFormat.nBlockAlign = soundData.blockAlign;
    waveFormat.nAvgBytesPerSec = soundData.bytesPerSecond;
    waveFormat.wBitsPerSample = soundData.bitsPerSample;
    waveFormat.cbSize = 0;

    DSBUFFERDESC secondaryBufferDesc = {};
    secondaryBufferDesc.dwSize = sizeof(DSBUFFERDESC);
    secondaryBufferDesc.dwFlags = DSBCAPS_CTRLVOLUME;
    secondaryBufferDesc.dwBufferBytes = soundData.data.size();
    secondaryBufferDesc.dwReserved = 0;
    secondaryBufferDesc.lpwfxFormat = &waveFormat;
    secondaryBufferDesc.guid3DAlgorithm = DS3DALG_DEFAULT;

    bool result = directSound->createSecondaryBuffer8(secondaryBuffer, secondaryBufferDesc, is3d);
    if (!result)
    {
        return false;
    }

    void* audioPointer1 = nullptr;
    uint32 audioSize1 = 0;
    void* audioPointer2 = nullptr;
    uint32 audioSize2 = 0;

    HRESULT hresult = secondaryBuffer->Lock(0, soundData.data.size(), &audioPointer1,
                                            reinterpret_cast<LPDWORD>(&audioSize1), &audioPointer2,
                                            reinterpret_cast<LPDWORD>(&audioSize2),
                                            DSBLOCK_FROMWRITECURSOR);
    if (FAILED(hresult))
    {
        return false;
    }

    if (!audioPointer2)
    {
        std::memcpy(audioPointer1, soundData.data.data(), soundData.data.size());
    }
    else
    {
        std::memcpy(audioPointer1, soundData.data.data(), audioSize1);
        std::memcpy(audioPointer2, soundData.data.data() + audioSize1, audioSize2);
    }

    hresult = secondaryBuffer->Unlock(audioPointer1, audioSize1, audioPointer2, audioSize2);
    if (FAILED(hresult))
    {
        return false;
    }

    return true;
}
