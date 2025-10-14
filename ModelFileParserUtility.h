#pragma once
#include <vector>
#include <unordered_map>

#include <string>

#include "Vertex.h"

#include "ImageFileParserUtility.h"

struct MaterialData
{
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

struct MeshData
{
    std::string name;

    std::vector<uint32> indexes;

    std::string materialName;
};

struct ModelData
{
    std::vector<Vertex> vertexes;

    std::vector<MeshData> meshDataItems;

    std::unordered_map<std::string, MaterialData> materialDataItems;
};
