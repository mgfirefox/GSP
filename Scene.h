#pragma once
#include <vector>
#include <unordered_map>

#include <string>

#include "Direct3d.h"

#include "SceneFileParser.h"

#include "Model.h"

#include "Transformation.h"

#include "SceneFileParserUtility.h"

class Scene
{
    bool initialized;
    bool released;

    std::shared_ptr<Direct3d> direct3d;

    std::shared_ptr<Shader> modelShader;

    SceneFileParser fileParser;

    std::vector<std::shared_ptr<Model>> models;

public:
    Scene(std::shared_ptr<Shader> modelShader, std::shared_ptr<Direct3d> direct3d);
    ~Scene();

private:
    bool isInitialized();
    void setInitialized();

    bool isReleased();
    void setReleased();

public:
    std::vector<std::shared_ptr<Model>> getModels();

    bool initialize(std::string filename);
    bool initialize(SceneData sceneData);
    bool render(DirectX::XMMATRIX vpMatrix);
    void release();

    void addModel(std::shared_ptr<Model> model);

private:
    bool readModels(std::string filename, SceneData& sceneData);
    bool initializeModels(SceneData sceneData);
};
