#pragma once
#include <unordered_map>
#include <vector>

#include <string>

#include "Transformation.h"

#include "ModelFileParserUtility.h"

struct SceneModelData
{
    std::string filename;

    Transformation transformation;
};

struct SceneData
{
    std::unordered_map<std::string, ModelData> uniqueModelDataItems;

    std::vector<SceneModelData> modelDataItems;
};
