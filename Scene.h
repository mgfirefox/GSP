#pragma once
#include <string>
#include <fstream>
#include <sstream>

#include <vector>
#include <unordered_map>

#include <DirectXMath.h>

#include <wrl/client.h>

#include "Model.h"

#include "FileParsers.h"

#include "transformation.h"

class Scene {
    bool initialized;
    bool released;

    std::vector<std::shared_ptr<Model>> models;

public:
    Scene();
    ~Scene();

private:
    bool isInitialized();
    void setInitialized();

    bool isReleased();
    void setReleased();

public:
    std::vector<std::shared_ptr<Model>> getModels();
    void setModels(std::vector<std::shared_ptr<Model>> models);

    bool initialize(Microsoft::WRL::ComPtr<ID3D11Device> device, Microsoft::WRL::ComPtr<ID3D11DeviceContext> deviceContext, std::string filename);
    bool initialize(Microsoft::WRL::ComPtr<ID3D11Device> device, Microsoft::WRL::ComPtr<ID3D11DeviceContext> deviceContext, SceneData sceneData);
    void release();
};

