#include "SoundFileParser.h"

bool SoundFileParser::parseFile(std::string filename, SoundData& soundData)
{
    std::string format = getFileFormat(filename);
    if (format == "wav")
    {
        return parseWavFile(filename, soundData);
    }

    return false;
}

bool SoundFileParser::parseWavFile(std::string filename, SoundData& soundData)
{
    std::ifstream file(filename, std::ios::binary);
    if (!file.is_open())
    {
        return false;
    }

    bool isRiffHeaderRead = false;
    bool isFmtHeaderRead = false;
    bool isDataHeaderRead = false;

    while (true)
    {
        WavMagicNumber magicNumber = {};
        file.read(reinterpret_cast<char*>(&magicNumber), WavMagicNumberSize);
        if (file.eof())
        {
            break;
        }
        if (!file || file.gcount() != WavMagicNumberSize)
        {
            return false;
        }
        file.seekg(-WavMagicNumberSize, std::ios::cur);

        if (magicNumber == WavMagicNumberRiff)
        {
            if (!readWavRiffHeader(file))
            {
                return false;
            }

            isRiffHeaderRead = true;
        }
        else if (magicNumber == WavMagicNumberFmt)
        {
            if (!readWavFmtHeader(file, soundData))
            {
                return false;
            }

            isFmtHeaderRead = true;
        }
        else if (magicNumber == WavMagicNumberData)
        {
            if (!readWavDataHeader(file, soundData))
            {
                return false;
            }

            isDataHeaderRead = true;
        }
        else
        {
            if (!skipWavUnknownHeader(file))
            {
                return false;
            }
        }
    }
    file.close();

    if (!(isRiffHeaderRead && isFmtHeaderRead && isDataHeaderRead))
    {
        return false;
    }

    return true;
}

bool SoundFileParser::readWavRiffHeader(std::ifstream& file)
{
    WavRiffHeader riffHeader = {};

    file.read(reinterpret_cast<char*>(&riffHeader), sizeof(WavRiffHeader));
    if (!file || file.gcount() != sizeof(WavRiffHeader))
    {
        return false;
    }
    if (riffHeader.format != WavMagicNumberWave)
    {
        return false;
    }

    return true;
}

bool SoundFileParser::readWavFmtHeader(std::ifstream& file, SoundData& soundData)
{
    WavFmtHeader fmtHeader = {};

    file.read(reinterpret_cast<char*>(&fmtHeader), sizeof(WavFmtHeader));
    if (!file || file.gcount() != sizeof(WavFmtHeader))
    {
        return false;
    }
    if (fmtHeader.audioFormat != WavAudioFormatPcm)
    {
        return false;
    }

    soundData.format = WAVE_FORMAT_PCM;
    soundData.numChannels = fmtHeader.numChannels;
    soundData.sampleRate = fmtHeader.sampleRate;
    soundData.bytesPerSecond = fmtHeader.bytesPerSecond;
    soundData.blockAlign = fmtHeader.blockAlign;
    soundData.bitsPerSample = fmtHeader.bitsPerSample;

    return true;
}

bool SoundFileParser::readWavDataHeader(std::ifstream& file, SoundData& soundData)
{
    WavDataHeader dataHeader = {};

    file.read(reinterpret_cast<char*>(&dataHeader), sizeof(WavDataHeader));
    if (!file || file.gcount() != sizeof(WavDataHeader))
    {
        return false;
    }

    soundData.data = std::vector<unsigned char>(dataHeader.subchunkSize);
    file.read(reinterpret_cast<char*>(soundData.data.data()), dataHeader.subchunkSize);
    if (!file || file.gcount() != dataHeader.subchunkSize)
    {
        return false;
    }

    return true;
}

bool SoundFileParser::skipWavUnknownHeader(std::ifstream& file)
{
    WavUnknownHeader unknownHeader = {};

    file.read(reinterpret_cast<char*>(&unknownHeader), sizeof(WavUnknownHeader));
    if (!file || file.gcount() != sizeof(WavUnknownHeader))
    {
        return false;
    }

    file.seekg(unknownHeader.subchunkSize, std::ios::cur);
    if (!file)
    {
        return false;
    }

    return true;
}
