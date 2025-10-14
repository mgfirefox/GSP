#include "Xaudio2.h"

Xaudio2::Xaudio2() : waveFormat{} // listener3dPosition{}
{
    initialized = false;
    released = false;
}

Xaudio2::~Xaudio2()
{
    release();
}

bool Xaudio2::isInitialized()
{
    return initialized;
}

void Xaudio2::setInitialized()
{
    initialized = true;
    released = false;
}

bool Xaudio2::isReleased()
{
    return released;
}

void Xaudio2::setReleased()
{
    initialized = false;
    released = true;
}

Microsoft::WRL::ComPtr<IXAudio2> Xaudio2::getXaudio2()
{
    return xaudio2;
}

std::shared_ptr<IXAudio2MasteringVoice> Xaudio2::getMasteringVoice()
{
    return masteringVoice;
}

bool Xaudio2::initialize(WAVEFORMATEX waveFormat)
{
    if (isInitialized())
    {
        release();
    }

    bool result = initializeXaudio2();
    if (!result)
    {
        return false;
    }

    this->waveFormat = waveFormat;

    result = initializeMasteringVoice();
    if (!result)
    {
        return false;
    }

    setInitialized();
    return true;
}

void Xaudio2::release()
{
    if (isReleased())
    {
        return;
    }

    waveFormat = {};

    masteringVoice.reset();

    xaudio2.Reset();

    setReleased();
}

bool Xaudio2::createSourceVoice(
    IXAudio2SourceVoice** sourceVoice, WAVEFORMATEX waveFormat, bool is3d)
{
    HRESULT result = xaudio2->CreateSourceVoice(sourceVoice, &waveFormat, 0, XAUDIO2_DEFAULT_FREQ_RATIO, nullptr, nullptr, nullptr);
    if (FAILED(result))
    {
        return false;
    }

    return true;
}

bool Xaudio2::initializeXaudio2()
{
    HRESULT result = XAudio2Create(xaudio2.GetAddressOf(), XAUDIO2_USE_DEFAULT_PROCESSOR);
    if (FAILED(result))
    {
        return false;
    }

    return true;
}

bool Xaudio2::initializeMasteringVoice()
{
    uint32 channelCount = waveFormat.nChannels == 0 ? XAUDIO2_DEFAULT_CHANNELS : waveFormat.nChannels;
    uint32 sampleRate = waveFormat.nSamplesPerSec == 0 ? XAUDIO2_DEFAULT_SAMPLERATE : waveFormat.nSamplesPerSec;
    
    IXAudio2MasteringVoice* masteringVoice = nullptr;
    
    HRESULT result = xaudio2->CreateMasteringVoice(&masteringVoice, channelCount, sampleRate, 0, nullptr, nullptr);
    if (FAILED(result))
    {        
        return false;
    }

    this->masteringVoice = std::shared_ptr<IXAudio2MasteringVoice>(masteringVoice, Xaudio2Deleter<IXAudio2MasteringVoice>());

    XAUDIO2_VOICE_DETAILS voiceDetails;

    this->masteringVoice->GetVoiceDetails(&voiceDetails);

    waveFormat.wFormatTag = WAVE_FORMAT_IEEE_FLOAT;
    if (waveFormat.nChannels == 0)
    {
        waveFormat.nChannels = voiceDetails.InputChannels;
    }
    if (waveFormat.nSamplesPerSec == 0)
    {
        waveFormat.nSamplesPerSec = voiceDetails.InputSampleRate;
    }
    // default waveFormat.wBitsPerSample cannot be retrieved so next fields cannot be calculated
    //waveFormat.nBlockAlign = waveFormat.wBitsPerSample / 8 * waveFormat.nChannels;
    //waveFormat.nAvgBytesPerSec = waveFormat.nSamplesPerSec * waveFormat.nBlockAlign;

    return true;
}
