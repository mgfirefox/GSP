#pragma once
#include <fstream>

#include <vector>

#include <string>

#include "DdsUtility.h"

#include "FileParserUtility.h"
#include "ImageFileParserUtility.h"

class ImageFileParser
{
public:
    bool parseFile(std::string filename, ImageData& imageData);

    bool parseDdsFile(std::string filename, ImageData& imageData);
};
