#include "Scene.h"

Scene::Scene() : models() {
    initialized = false;
    released = false;
}

Scene::~Scene() {
    release();
}

bool Scene::isInitialized() {
    return initialized;
}

void Scene::setInitialized() {
    initialized = true;
    released = false;
}

bool Scene::isReleased() {
    return released;
}

void Scene::setReleased() {
    initialized = false;
    released = true;
}

std::vector<std::shared_ptr<Model>> Scene::getModels() {
    return models;
}

void Scene::setModels(std::vector<std::shared_ptr<Model>> models) {
    this->models = models;
}

bool Scene::initialize(Microsoft::WRL::ComPtr<ID3D11Device> device, Microsoft::WRL::ComPtr<ID3D11DeviceContext> deviceContext, std::string filename) {
    if (isInitialized()) {
        release();
    }

    SceneData sceneData = {};

    SceneFileParser sceneFileParser;
    bool result = sceneFileParser.parseFile(filename, sceneData);
    if (!result) {
        return false;
    }

    result = initialize(device, deviceContext, sceneData);
    if (!result) {
        return false;
    }

    setInitialized();
    return true;
}

bool Scene::initialize(Microsoft::WRL::ComPtr<ID3D11Device> device, Microsoft::WRL::ComPtr<ID3D11DeviceContext> deviceContext, SceneData sceneData) {
    std::unordered_map<std::string, std::unique_ptr<Model>> uniqueModels;
    for (const auto& pair : sceneData.modelDataItems) {
        std::unique_ptr<Model>& uniqueModel = uniqueModels[pair.first];
        uniqueModel = std::make_unique<Model>();
        bool result = uniqueModel->initialize(device, deviceContext, pair.second);
        if (!result) {
            return false;
        }
    }

    for (const auto& sceneModelData : sceneData.sceneModelDataItems) {
        std::unique_ptr<Model>& uniqueModel = uniqueModels[sceneModelData.filename];
        if (!uniqueModel) {
            continue;
        }

        std::shared_ptr<Model> model(new Model(*uniqueModel), Model::deleter);
        model->setTransformation(sceneModelData.transformation);
        models.push_back(model);
    }

    for (auto& pair : uniqueModels) {
        pair.second->release();
        pair.second.reset();
    }
    uniqueModels.clear();

    setInitialized();
    return true;
}

void Scene::release() {
    if (isReleased()) {
        return;
    }

    for (auto& model : models) {
        model.reset();
    }
    models.clear();

    setReleased();
}
