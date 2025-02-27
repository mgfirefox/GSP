#pragma once
#include <d3d11.h>
#include <DirectXMath.h>

#include <wrl/client.h>

#include <fstream>
#include <string>

#include "model_loader.h"
#include "transformation.h"

class Model {
    bool initialized;
    bool released;

    Microsoft::WRL::ComPtr<ID3D11Buffer> vertexBuffer;
    Microsoft::WRL::ComPtr<ID3D11Buffer> indexBuffer;

    std::vector<ml::Vertex> vertexes;
    std::vector<unsigned long> indexes;

    unsigned long vertexesQuantity;
    unsigned long indexesQuantity;

    Transformation transformation;

public:
    Model();
    Model(const Model& model);
    ~Model();

private:
    bool isInitialized();
    void setInitialized();

    bool isReleased();
    void setReleased();

public:
    Transformation getTransformation();
    void setTransformation(Transformation transformation);

    bool initialize(Microsoft::WRL::ComPtr<ID3D11Device> device, std::string filename, Transformation transformation = {});
    void render(Microsoft::WRL::ComPtr<ID3D11DeviceContext> deviceContext);
    void release();

private:
    bool loadModel(std::string filename);
    void releaseModel();

    bool initializeBuffers(Microsoft::WRL::ComPtr<ID3D11Device> device);
    void renderBuffers(Microsoft::WRL::ComPtr<ID3D11DeviceContext> deviceContext);
    void releaseBuffers();
};
