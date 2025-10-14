#pragma once
#include <dsound.h>

#include <wrl/client.h>
#include <memory>

#include <string>

#include "Xaudio2.h"

#include "SoundFileParser.h"

#include "IntUtility.h"

#include "SoundUtility.h"
#include "SoundFileParserUtility.h"

class Xaudio2Sound
{
    bool initialized;
    bool released;

protected:
    std::shared_ptr<Xaudio2> xaudio2;

    SoundFileParser fileParser;

    std::vector<unsigned char> bufferData;
    XAUDIO2_BUFFER buffer;
    std::shared_ptr<IXAudio2SourceVoice> sourceVoice;

    float volume;

private:
    bool looping;

    SoundState state;

    bool muted;

public:
    Xaudio2Sound(std::shared_ptr<Xaudio2> xaudio2);
    virtual ~Xaudio2Sound();

private:
    bool isInitialized();
    void setInitialized();

    bool isReleased();
    void setReleased();

public:
    std::shared_ptr<IXAudio2SourceVoice> getSourceVoice();

    virtual bool setVolume(float volume);

    bool isLooping();

    SoundState getState();
    bool isPlaying();
    bool isPaused();
    bool isStopped();

    bool isMuted();

    bool initialize(std::string filename, bool isLooping = false, float volume = 1.0f, bool is3d = false);
    bool initialize(SoundData soundData, bool isLooping = false, float volume = 1.0f, bool is3d = false);
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
    bool initializeBuffer(SoundData soundData);
    virtual bool initializeSourceVoice(SoundData soundData, bool is3d);
};
