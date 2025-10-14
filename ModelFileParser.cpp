#include "ModelFileParser.h"

bool ModelFileParser::parseFile(std::string filename, ModelData& modelData)
{
    std::string format = getFileFormat(filename);
    if (format == "obj")
    {
        return parseObjFile(filename, modelData);
    }

    return false;
}

bool ModelFileParser::parseObjFile(std::string filename, ModelData& modelData)
{
    std::ifstream file(filename);
    if (!file.is_open())
    {
        return false;
    }

    std::string line;
    while (std::getline(file, line))
    {
        std::istringstream lineStream(line);

        std::string keyword;
        lineStream >> keyword;

        if (keyword == "mtllib")
        {
            lineStream >> std::ws;

            std::string mtlFilename;
            std::getline(lineStream, mtlFilename);

            bool result = parseMtlFile(mtlFilename, modelData);
            if (!result)
            {
                return false;
            }

            break;
        }
    }

    MeshData meshData = {};

    std::vector<DirectX::XMFLOAT3> positions;
    std::vector<DirectX::XMFLOAT3> textureCoordinatesItems;
    std::vector<DirectX::XMFLOAT3> normals;

    std::unordered_map<Vertex, uint32> uniqueVertexes;

    while (std::getline(file, line))
    {
        std::istringstream lineStream(line);

        std::string keyword;
        lineStream >> keyword;

        if (keyword == "v")
        {
            DirectX::XMFLOAT3 position = {};
            lineStream >> position.x >> position.y >> position.z;
            positions.push_back(position);
        }
        else if (keyword == "vn")
        {
            DirectX::XMFLOAT3 normal = {};
            lineStream >> normal.x >> normal.y >> normal.z;
            normals.push_back(normal);
        }
        else if (keyword == "vt")
        {
            DirectX::XMFLOAT3 textureCoordinates = {};
            lineStream >> textureCoordinates.x >> textureCoordinates.y >> textureCoordinates.z;
            textureCoordinatesItems.push_back(textureCoordinates);
        }
        else if (keyword == "o")
        {
            if (!meshData.name.empty() && !meshData.indexes.empty() && !meshData.materialName.
                empty())
            {
                modelData.meshDataItems.push_back(meshData);

                meshData = {};
            }

            lineStream >> std::ws;
            std::getline(lineStream, meshData.name);
        }
        else if (keyword == "usemtl")
        {
            lineStream >> std::ws;
            std::getline(lineStream, meshData.materialName);
        }
        else if (keyword == "f")
        {
            for (int32 i = 0; i < 3; i++)
            {
                std::string vertexDataIndexes;
                lineStream >> vertexDataIndexes;

                std::istringstream vertexDataIndexesStream(vertexDataIndexes);

                std::string positionIndex;
                std::getline(vertexDataIndexesStream, positionIndex, '/');

                std::string textureCoordinatesIndex;
                std::getline(vertexDataIndexesStream, textureCoordinatesIndex, '/');

                std::string normalIndex;
                std::getline(vertexDataIndexesStream, normalIndex, ' ');

                Vertex vertex = {};
                try
                {
                    vertex.position = positions[std::stoull(positionIndex) - 1];
                    vertex.textureCoordinates = textureCoordinatesItems[
                        std::stoull(textureCoordinatesIndex) - 1];
                    vertex.normal = normals[std::stoull(normalIndex) - 1];
                }
                catch (const std::invalid_argument&)
                {
                    return false;
                }
                catch (const std::out_of_range&)
                {
                    return false;
                }

                if (uniqueVertexes.count(vertex) == 0)
                {
                    uniqueVertexes[vertex] = modelData.vertexes.size();

                    modelData.vertexes.push_back(vertex);
                }

                meshData.indexes.push_back(uniqueVertexes[vertex]);
            }

            lineStream >> std::ws;
            if (!lineStream.eof())
            {
                return false;
            }
        }
    }

    file.close();

    if (!meshData.name.empty())
    {
        if (meshData.indexes.empty())
        {
            return false;
        }

        modelData.meshDataItems.push_back(meshData);
    }

    return true;
}

bool ModelFileParser::parseMtlFile(std::string filename, ModelData& modelData)
{
    std::ifstream file(filename);
    if (!file.is_open())
    {
        return false;
    }

    MaterialData materialData = {};

    std::string line;
    while (std::getline(file, line))
    {
        std::istringstream lineStream(line);

        std::string keyword;
        lineStream >> keyword;

        if (keyword == "newmtl")
        {
            if (!materialData.name.empty())
            {
                modelData.materialDataItems[materialData.name] = materialData;

                materialData = {};
            }

            lineStream >> std::ws;
            std::getline(lineStream, materialData.name);
        }
        else if (keyword == "Kd")
        {
            lineStream >> materialData.diffuseColor.x >> materialData.diffuseColor.y >> materialData
                .diffuseColor.z;
        }
        else if (keyword == "d")
        {
            lineStream >> materialData.opacity;
        }
        else if (keyword == "Tr")
        {
            float transparency;
            lineStream >> transparency;

            materialData.opacity = 1.0f - transparency;
        }
        else if (keyword == "map_Kd")
        {
            materialData.hasDiffuseColorImage = true;

            std::string diffuseColorFilename;

            lineStream >> std::ws;
            std::getline(lineStream, diffuseColorFilename);

            bool result = imageFileParser.parseFile(diffuseColorFilename,
                                                    materialData.diffuseColorImageData);
            if (!result)
            {
                return false;
            }
        }
    }

    file.close();

    if (materialData.name.empty())
    {
        return true;
    }

    modelData.materialDataItems[materialData.name] = materialData;

    return true;
}
