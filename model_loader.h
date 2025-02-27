#pragma once
#include <string>
#include <fstream>
#include <sstream>

#include <vector>
#include <unordered_map>

#include <DirectXMath.h>

namespace ml {
    struct Vertex {
        DirectX::XMFLOAT3 position;
        DirectX::XMFLOAT3 textureCoordinates;
        DirectX::XMFLOAT3 normals;

        bool operator==(const Vertex& vertex) const {
            return position.x == vertex.position.x && position.y == vertex.position.y && position.z == vertex.position.z
                && textureCoordinates.x == vertex.textureCoordinates.x && textureCoordinates.y == vertex.textureCoordinates.y && textureCoordinates.z == vertex.textureCoordinates.z
                && normals.x == vertex.normals.x && normals.y == vertex.normals.y && normals.z == vertex.normals.z;
        }
    };

    bool loadObj(std::string filename, std::vector<Vertex>& vertexes, std::vector<unsigned long>& indexes);
}

namespace std {
    template <>
    struct hash<ml::Vertex> {
        size_t operator()(const ml::Vertex& vertex) const {
            return hash<float>()(vertex.position.x) ^ hash<float>()(vertex.position.y) ^ hash<float>()(vertex.position.z)
                ^ hash<float>()(vertex.textureCoordinates.x) ^ hash<float>()(vertex.textureCoordinates.y) ^ hash<float>()(vertex.textureCoordinates.z)
                ^ hash<float>()(vertex.normals.x) ^ hash<float>()(vertex.normals.y) ^ hash<float>()(vertex.normals.z);
        }
    };
}