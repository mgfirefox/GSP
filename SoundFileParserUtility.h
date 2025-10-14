#pragma once
#include <vector>

#include "IntUtility.h"

struct SoundData
{
    uint16 format;
    uint16 numChannels;
    uint16 sampleRate;
    uint32 bytesPerSecond;
    uint16 blockAlign;
    uint16 bitsPerSample;

    std::vector<unsigned char> data;
};
