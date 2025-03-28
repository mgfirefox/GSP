#include "model_loader.h"

bool ml::loadObj(std::string filename, std::vector<Vertex>& vertexes, std::vector<unsigned int>& indexes) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        return false;
    }

    std::vector<DirectX::XMFLOAT3> positions;
    std::vector<DirectX::XMFLOAT3> textureCoordinatesItems;
    std::vector<DirectX::XMFLOAT3> normalsItems;

    std::unordered_map<Vertex, unsigned int> uniqueVertexes;

    std::string line;
    while (std::getline(file, line)) {
        std::istringstream lineStream(line);

        std::string keyword;
        lineStream >> keyword;

        if (keyword == "v") {
            DirectX::XMFLOAT3 position = {};
            lineStream >> position.x >> position.y >> position.z;

            positions.push_back(position);
        }
        else if (keyword == "vt") {
            DirectX::XMFLOAT3 textureCoordinates = {};
            lineStream >> textureCoordinates.x >> textureCoordinates.y >> textureCoordinates.z;

            textureCoordinatesItems.push_back(textureCoordinates);
        }
        else if (keyword == "vn") {
            DirectX::XMFLOAT3 normals = {};
            lineStream >> normals.x >> normals.y >> normals.z;

            normalsItems.push_back(normals);
        }
        else if (keyword == "f") {
            for (int i = 0; i < 3; i++) {
                std::string vertexDataIndexes;
                lineStream >> vertexDataIndexes;

                std::istringstream vertexDataStream(vertexDataIndexes);

                std::string positionIndex;
                std::getline(vertexDataStream, positionIndex, '/');

                std::string textureCoordinatesIndex;
                std::getline(vertexDataStream, textureCoordinatesIndex, '/');

                std::string normalsIndex;
                std::getline(vertexDataStream, normalsIndex, ' ');

                Vertex vertex = {};
                vertex.position = positions[std::stoul(positionIndex) - 1];
                vertex.textureCoordinates = textureCoordinatesItems[std::stoul(textureCoordinatesIndex) - 1];
                vertex.normals = normalsItems[std::stoul(normalsIndex) - 1];

                if (uniqueVertexes.count(vertex) == 0) {
                    uniqueVertexes[vertex] = (unsigned int)vertexes.size();
                    vertexes.push_back(vertex);
                }
                indexes.push_back(uniqueVertexes[vertex]);
            }
        }
    }
    file.close();

    return true;
}
