#include "Xaudio2Sound.h"

Xaudio2Sound::Xaudio2Sound(std::shared_ptr<Xaudio2> xaudio2) : fileParser(), bufferData(), buffer{}, sourceVoice()
{
    initialized = false;
    released = false;

    this->xaudio2 = xaudio2;

    volume = 0.0f;

    looping = false;

    state = SoundState::Undefined;

    muted = false;
}

Xaudio2Sound::~Xaudio2Sound()
{
    Xaudio2Sound::release();

    xaudio2.reset();
}

bool Xaudio2Sound::isInitialized()
{
    return initialized;
}

void Xaudio2Sound::setInitialized()
{
    initialized = true;
    released = false;
}

bool Xaudio2Sound::isReleased()
{
    return released;
}

void Xaudio2Sound::setReleased()
{
    initialized = false;
    released = true;
}

std::shared_ptr<IXAudio2SourceVoice> Xaudio2Sound::getSourceVoice()
{
    return sourceVoice;
}

bool Xaudio2Sound::setVolume(float volume)
{
    if (!muted)
    {
        HRESULT result = sourceVoice->SetVolume(volume, XAUDIO2_COMMIT_NOW);
        if (FAILED(result))
        {
            return false;
        }
    }

    this->volume = volume;

    return true;
}

bool Xaudio2Sound::isLooping()
{
    return looping;
}

SoundState Xaudio2Sound::getState()
{
    updateState();

    return state;
}

bool Xaudio2Sound::isPlaying()
{
    return getState() == SoundState::Playing;
}

bool Xaudio2Sound::isPaused()
{
    return getState() == SoundState::Paused;
}

bool Xaudio2Sound::isStopped()
{
    return getState() == SoundState::Stopped;
}

bool Xaudio2Sound::isMuted()
{
    return muted;
}

bool Xaudio2Sound::initialize(std::string filename, bool isLooping, float volume, bool is3d)
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

    looping = isLooping;

    result = initializeBuffer(soundData);
    if (!result)
    {
        return false;
    }
    
    result = initializeSourceVoice(soundData, is3d);
    if (!result)
    {
        return false;
    }
    
    result = setVolume(volume);
    if (!result)
    {
        return false;
    }
    
    state = SoundState::Stopped;

    setInitialized();
    return true;
}

bool Xaudio2Sound::initialize(SoundData soundData, bool isLooping, float volume, bool is3d)
{
    if (isInitialized())
    {
        release();
    }

    looping = isLooping;
    
    bool result = initializeSourceVoice(soundData, is3d);
    if (!result)
    {
        return false;
    }
    
    result = setVolume(volume);
    if (!result)
    {
        return false;
    }
    
    state = SoundState::Stopped;

    setInitialized();
    return true;
}

void Xaudio2Sound::release()
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

    sourceVoice.reset();
    buffer = {};
    bufferData.clear();
    bufferData.shrink_to_fit();

    setReleased();
}

bool Xaudio2Sound::play()
{
    if (!isPlaying())
    {
        HRESULT result = sourceVoice->Start(0, XAUDIO2_COMMIT_NOW);
        if (FAILED(result))
        {
            return false;
        }
        
        this->state = SoundState::Playing;
    }

    return true;
}

bool Xaudio2Sound::pause()
{
    SoundState state = getState();
    
    if (state != SoundState::Paused && state != SoundState::Stopped)
    {
        HRESULT result = sourceVoice->Stop(0, XAUDIO2_COMMIT_NOW);
        if (FAILED(result))
        {
            return false;
        }

        this->state = SoundState::Paused;
    }

    return true;
}

bool Xaudio2Sound::stop()
{
    if (!isStopped())
    {
        HRESULT result = sourceVoice->Stop(0, XAUDIO2_COMMIT_NOW);
        if (FAILED(result))
        {
            return false;
        }

        result = sourceVoice->FlushSourceBuffers();
        if (FAILED(result))
        {
            return false;
        }

        result = sourceVoice->SubmitSourceBuffer(&buffer);
        if (FAILED(result))
        {
            return false;
        }

        state = SoundState::Stopped;
    }

    return true;
}

bool Xaudio2Sound::mute()
{
    if (!isMuted())
    {
        HRESULT result = sourceVoice->SetVolume(0.0f, XAUDIO2_COMMIT_NOW);
        if (FAILED(result))
        {
            return false;
        }
        
        muted = true;
    }


    return true;
}

bool Xaudio2Sound::unmute()
{
    if (isMuted())
    {
        HRESULT result = sourceVoice->SetVolume(volume, XAUDIO2_COMMIT_NOW);
        if (FAILED(result))
        {
            return false;
        }
        
        muted = false;
    }


    return true;
}

void Xaudio2Sound::updateState()
{
    if (state != SoundState::Playing)
    {
        return;
    }

    XAUDIO2_VOICE_STATE voiceState = {};

    sourceVoice->GetState(&voiceState, XAUDIO2_VOICE_NOSAMPLESPLAYED);

    if (voiceState.BuffersQueued > 0)
    {
        return;
    }

    state = SoundState::Stopped;
}

bool Xaudio2Sound::readData(std::string filename, SoundData& soundData)
{
    bool result = fileParser.parseFile(filename, soundData);
    if (!result)
    {
        return false;
    }

    return true;
}

bool Xaudio2Sound::initializeBuffer(SoundData soundData)
{
    bufferData = soundData.data;
    
    buffer.Flags = XAUDIO2_END_OF_STREAM;
    buffer.AudioBytes = bufferData.size();
    buffer.pAudioData = bufferData.data();
    buffer.PlayBegin = 0;
    buffer.PlayLength = 0;
    buffer.LoopBegin = 0;
    buffer.LoopLength = 0;
    buffer.LoopCount = looping ? XAUDIO2_MAX_LOOP_COUNT : 0;
    buffer.pContext = nullptr;

    return true;
}

bool Xaudio2Sound::initializeSourceVoice(SoundData soundData, bool is3d)
{
    WAVEFORMATEX waveFormat = {};
    waveFormat.wFormatTag = soundData.format;
    waveFormat.nChannels = soundData.numChannels;
    waveFormat.nSamplesPerSec = soundData.sampleRate;
    waveFormat.nBlockAlign = soundData.blockAlign;
    waveFormat.nAvgBytesPerSec = soundData.bytesPerSecond;
    waveFormat.wBitsPerSample = soundData.bitsPerSample;
    waveFormat.cbSize = 0;

    IXAudio2SourceVoice* sourceVoice = nullptr;

    bool result = xaudio2->createSourceVoice(&sourceVoice, waveFormat, is3d);
    if (!result)
    {        
        return false;
    }

    this->sourceVoice = std::shared_ptr<IXAudio2SourceVoice>(sourceVoice, Xaudio2Deleter<IXAudio2SourceVoice>());

    HRESULT hresult = this->sourceVoice->SubmitSourceBuffer(&buffer);
    if (FAILED(hresult))
    {
        return false;
    }

    return true;
}
