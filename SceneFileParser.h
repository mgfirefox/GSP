#pragma once
#include <fstream>
#include <sstream>

#include <string>

#include <vector>
#include <unordered_map>

#include "Vertex.h"
#include "Transformation.h"

#include "ModelFileParser.h"

#include "FileParserUtility.h"
#include "SceneFileParserUtility.h"

class SceneFileParser
{
    ModelFileParser modelFileParser;

public:
    bool parseFile(std::string filename, SceneData& sceneData);

    bool parseSceneFile(std::string filename, SceneData& sceneData);
};
