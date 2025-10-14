#pragma once
#include <xaudio2.h>
//#include <DirectXMath.h>

#include <wrl/client.h>
#include <memory>

#include "IntUtility.h"

#include "MemoryUtility.h"

#include "Xaudio2Utility.h"

class Xaudio2
{
    bool initialized;
    bool released;

    Microsoft::WRL::ComPtr<IXAudio2> xaudio2;
    std::shared_ptr<IXAudio2MasteringVoice> masteringVoice;

    WAVEFORMATEX waveFormat;

public:
    Xaudio2();
    ~Xaudio2();

private:
    bool isInitialized();
    void setInitialized();

    bool isReleased();
    void setReleased();

public:
    Microsoft::WRL::ComPtr<IXAudio2> getXaudio2();
    std::shared_ptr<IXAudio2MasteringVoice> getMasteringVoice();

    bool initialize(WAVEFORMATEX waveFormat);
    void release();

    bool createSourceVoice(IXAudio2SourceVoice** sourceVoice,
                           WAVEFORMATEX waveFormat, bool is3d);

private:
    bool initializeXaudio2();
    bool initializeMasteringVoice();
};
