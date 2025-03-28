#pragma once
#include <DirectXMath.h>

#include <string>
#include <fstream>
#include <sstream>

#include <vector>
#include <unordered_map>

#include <d3d11.h>

#include "vertex.h"

#include "dds.h"

#include "transformation.h"

struct MipmapData {
    unsigned int rowPitch;
    unsigned int depthPitch;

    std::vector<unsigned char> data;
};

struct ImageData {
    unsigned int width;
    unsigned int height;
    unsigned int depth;

    DXGI_FORMAT format;

    unsigned int mipmapLevels;
    std::vector<MipmapData> mipmapDataItems;
};

struct MaterialData {
    std::string name;

    DirectX::XMFLOAT3 ambientColor; // Ka;
    DirectX::XMFLOAT3 diffuseColor; // Kd;
    DirectX::XMFLOAT3 specularColor; // Ks;

    ImageData ambientColorImageData;
    bool hasDiffuseColorImage;
    ImageData diffuseColorImageData;
    ImageData specularColorImageData;

    float opacity; // Tr / inverted d;
    DirectX::XMFLOAT3 transmissionFilterColor; // Tf;

    /*float Pr;
    float Pm;
    float Pc;
    float Pcr;*/

    float indexOfRefraction; // Ni;
    // int glareHardness; // Ns;

    DirectX::XMFLOAT3 emissionColor; // Ke;

    int illuminationModel; // illum;

    //std::string PrFilename;
    std::string displacementFilename;
    std::string bumpFilename;
};

struct MtlMaterialData {
    std::unordered_map<std::string, MaterialData> materialsDataItems;
};

struct MeshData {
    std::string name;
    std::vector<unsigned int> indexes;
    std::string materialName;
};

struct ModelData {
    std::vector<Vertex> vertexes;
    std::vector<MeshData> meshDataItems;
    std::unordered_map<std::string, MaterialData> materialsDataItems;
};

struct SceneModelData {
    std::string filename;
    Transformation transformation;
};

struct SceneData {
    std::vector<SceneModelData> sceneModelDataItems;
    std::unordered_map<std::string, ModelData> modelDataItems;
};

std::string getFileFormat(std::string filename);

struct ImageFileParser {
    bool parseFile(std::string filename, ImageData& imageData);

    bool parseDdsFile(std::string filename, ImageData& imageData);
};

struct ModelFileParser {
    bool parseFile(std::string filename, ModelData& modelData);

    bool parseObjFile(std::string filename, ModelData& modelData);
    bool parseMtlFile(std::string filename, ModelData& modelData);
};

struct SceneFileParser {
    bool parseFile(std::string filename, SceneData& sceneData);

    bool parseSceneFile(std::string filename, SceneData& sceneData);
};
