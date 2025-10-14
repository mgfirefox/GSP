#pragma once
#include "IntUtility.h"

constexpr int32 WavMagicNumberSize = 4;

union WavMagicNumber
{
    uint32 number;
    unsigned char chars[WavMagicNumberSize];
};

constexpr WavMagicNumber WavMagicNumberRiff = {0x46464952}; // 'RIFF'
constexpr WavMagicNumber WavMagicNumberWave = {0x45564157}; // 'WAVE'
constexpr WavMagicNumber WavMagicNumberFmt = {0x20746d66}; // 'fmt '
constexpr WavMagicNumber WavMagicNumberData = {0x61746164}; // 'data'

constexpr uint32 WavAudioFormatPcm = 1;

struct WavRiffHeader
{
    WavMagicNumber chunkId;
    uint32 chunkSize;
    WavMagicNumber format;
};

struct WavFmtHeader
{
    WavMagicNumber subchunkId;
    uint32 subchunkSize;
    uint16 audioFormat;
    uint16 numChannels;
    uint32 sampleRate;
    uint32 bytesPerSecond;
    uint16 blockAlign;
    uint16 bitsPerSample;
};

struct WavDataHeader
{
    WavMagicNumber subchunkId;
    uint32 subchunkSize;
};

struct WavUnknownHeader
{
    WavMagicNumber subchunkId;
    uint32 subchunkSize;
};

inline bool operator==(const WavMagicNumber& lhs, const WavMagicNumber& rhs)
{
    return lhs.number == rhs.number;
}

inline bool operator!=(const WavMagicNumber& lhs, const WavMagicNumber& rhs)
{
    return !(lhs == rhs);
}
