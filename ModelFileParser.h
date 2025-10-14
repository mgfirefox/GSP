#pragma once
#include <fstream>
#include <sstream>

#include <vector>
#include <unordered_map>

#include <string>

#include "Vertex.h"

#include "ImageFileParser.h"

#include "FileParserUtility.h"
#include "ModelFileParserUtility.h"
#include "ImageFileParserUtility.h"

class ModelFileParser
{
    ImageFileParser imageFileParser;

public:
    bool parseFile(std::string filename, ModelData& modelData);

    bool parseObjFile(std::string filename, ModelData& modelData);
    bool parseMtlFile(std::string filename, ModelData& modelData);
};
