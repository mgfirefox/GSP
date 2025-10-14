#pragma once
#include <d3d11.h>

#include <fstream>

#include <vector>

#include <string>

#include "WavUtility.h"

#include "FileParserUtility.h"
#include "SoundFileParserUtility.h"

class SoundFileParser
{
public:
    bool parseFile(std::string filename, SoundData& soundData);
    bool parseWavFile(std::string filename, SoundData& soundData);

private:
    bool readWavRiffHeader(std::ifstream& file);
    bool readWavFmtHeader(std::ifstream& file, SoundData& soundData);
    bool readWavDataHeader(std::ifstream& file, SoundData& soundData);
    bool skipWavUnknownHeader(std::ifstream& file);
};
