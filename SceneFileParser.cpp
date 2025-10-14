#include "SceneFileParser.h"

bool SceneFileParser::parseFile(std::string filename, SceneData& sceneData)
{
    std::string format = getFileFormat(filename);
    if (format == "scene")
    {
        return parseSceneFile(filename, sceneData);
    }

    return false;
}

bool SceneFileParser::parseSceneFile(std::string filename, SceneData& sceneData)
{
    std::ifstream file(filename);
    if (!file.is_open())
    {
        return false;
    }

    SceneModelData modelData = {};

    std::vector<std::string> filenames;
    std::vector<DirectX::XMFLOAT3> positions;
    std::vector<DirectX::XMFLOAT3> orientations;
    std::vector<DirectX::XMFLOAT3> scalingItems;

    std::string line;
    while (std::getline(file, line))
    {
        std::istringstream lineStream(line);

        std::string keyword;
        lineStream >> keyword;

        if (keyword == "f")
        {
            lineStream >> std::ws;

            std::string filename;
            std::getline(lineStream, filename);

            filenames.push_back(filename);
        }
        else if (keyword == "p")
        {
            DirectX::XMFLOAT3 position = {};
            lineStream >> position.x >> position.y >> position.z;
            positions.push_back(position);
        }
        else if (keyword == "o")
        {
            DirectX::XMFLOAT3 orientation = {};
            lineStream >> orientation.x >> orientation.y >> orientation.z;
            orientations.push_back(orientation);
        }
        else if (keyword == "s")
        {
            DirectX::XMFLOAT3 scaling = {};
            lineStream >> scaling.x >> scaling.y >> scaling.z;
            scalingItems.push_back(scaling);
        }
        else if (keyword == "m")
        {
            std::string modelDataIndexes;
            lineStream >> modelDataIndexes;

            std::istringstream modelDataStream(modelDataIndexes);

            std::string filenameIndex;
            std::getline(modelDataStream, filenameIndex, '/');

            std::string positionIndex;
            std::getline(modelDataStream, positionIndex, '/');

            std::string rotationIndex;
            std::getline(modelDataStream, rotationIndex, '/');

            std::string scalingIndex;
            std::getline(modelDataStream, scalingIndex, ' ');

            try
            {
                modelData.filename = filenames[std::stoull(
                    filenameIndex) - 1];

                modelData.transformation.position = positions[
                    std::stoull(
                        positionIndex) - 1];
                modelData.transformation.orientation = orientations[
                    std::stoull(
                        rotationIndex) - 1];
                modelData.transformation.scale_ = scalingItems[
                    std::stoull(
                        scalingIndex) - 1];
            }
            catch (const std::invalid_argument&)
            {
                return false;
            }
            catch (const std::out_of_range&)
            {
                return false;
            }

            sceneData.modelDataItems.push_back(modelData);

            if (sceneData.uniqueModelDataItems.count(modelData.filename) == 0)
            {
                ModelData uniqueModelData = {};

                bool result = modelFileParser.parseFile(modelData.filename, uniqueModelData);
                if (!result)
                {
                    return false;
                }

                sceneData.uniqueModelDataItems[modelData.filename] = uniqueModelData;
            }
        }
    }

    file.close();

    return true;
}
