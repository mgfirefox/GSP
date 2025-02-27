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

bool Scene::initialize(Microsoft::WRL::ComPtr<ID3D11Device> device, std::string filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        return false;
    }

    std::vector<std::string> filenames;
    std::vector<DirectX::XMFLOAT3> positions;
    std::vector<DirectX::XMFLOAT3> rotations;
    std::vector<DirectX::XMFLOAT3> scalingItems;

    std::unordered_map<std::string, std::shared_ptr<Model>> uniqueModels;

    std::string line;
    while (std::getline(file, line)) {
        std::istringstream lineStream(line);

        std::string keyword;
        lineStream >> keyword;

        if (keyword == "f") {
            std::string filenameData;
            lineStream >> filenameData;

            std::istringstream filenameDataStream(filenameData);

            std::string filename;
            std::getline(filenameDataStream, filename, '\n');

            filenames.push_back(filename);
        }
        else if (keyword == "p") {
            DirectX::XMFLOAT3 position = {};
            lineStream >> position.x >> position.y >> position.z;

            positions.push_back(position);
        }
        else if (keyword == "r") {
            DirectX::XMFLOAT3 rotation = {};
            lineStream >> rotation.x >> rotation.y >> rotation.z;

            rotations.push_back(rotation);
        }
        else if (keyword == "s") {
            DirectX::XMFLOAT3 scaling = {};
            lineStream >> scaling.x >> scaling.y >> scaling.z;

            scalingItems.push_back(scaling);
        }
        else if (keyword == "m") {
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

            std::string filename = filenames[std::stoul(filenameIndex) - 1];

            Transformation transformation = {};
            transformation.position = positions[std::stoul(positionIndex) - 1];
            transformation.rotation = rotations[std::stoul(rotationIndex) - 1];
            transformation.scaling = scalingItems[std::stoul(scalingIndex) - 1];

            std::shared_ptr<Model> model;
            if (uniqueModels.count(filename) == 0) {
                model = std::make_shared<Model>();
                bool result = model->initialize(device, filename);
                if (!result) {
                    continue;
                }

                uniqueModels[filename] = std::make_shared<Model>(*model);

                model->setTransformation(transformation);
            }
            else {
                model = std::make_shared<Model>(*uniqueModels[filename]);
                model->setTransformation(transformation);
            }
            models.push_back(model);
        }
    }
    file.close();

    setInitialized();
    return true;
}

void Scene::release() {
    for (const auto& model : models) {
        model->release();
    }

    setReleased();
}

