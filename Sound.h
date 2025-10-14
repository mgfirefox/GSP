#pragma once
#include <dsound.h>

#include <wrl/client.h>
#include <memory>

#include <string>

#include "DirectSound.h"

#include "SoundFileParser.h"

#include "IntUtility.h"

#include "SoundUtility.h"
#include "SoundFileParserUtility.h"

class Sound
{
    bool initialized;
    bool released;

protected:
    std::shared_ptr<DirectSound> directSound;

    SoundFileParser fileParser;

    Microsoft::WRL::ComPtr<IDirectSoundBuffer8> secondaryBuffer;

    int32 volume;

private:
    bool looping;

    SoundState state;

    bool muted;

public:
    Sound(std::shared_ptr<DirectSound> directSound);
    virtual ~Sound();

private:
    bool isInitialized();
    void setInitialized();

    bool isReleased();
    void setReleased();

public:
    Microsoft::WRL::ComPtr<IDirectSoundBuffer8> getSecondaryBuffer();

    virtual bool setVolume(int32 volume);

    bool isLooping();

    SoundState getState();
    bool isPlaying();
    bool isPaused();
    bool isStopped();

    bool isMuted();

    bool initialize(std::string filename, bool isLooping = false, int32 volume = DSBVOLUME_MAX, bool is3d = false);
    bool initialize(SoundData soundData, bool isLooping = false, int32 volume = DSBVOLUME_MAX, bool is3d = false);
    virtual void release();

    bool play();
    bool pause();
    bool stop();

    bool mute();
    bool unmute();

private:
    void updateState();

    bool readData(std::string filename, SoundData& soundData);

protected:
    virtual bool initializeSecondaryBuffer8(SoundData soundData, bool is3d);
};
