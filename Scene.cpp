#include "Scene.h"

Scene::Scene(std::shared_ptr<Shader> modelShader,
             std::shared_ptr<Direct3d> direct3d) : fileParser(), models()
{
    initialized = false;
    released = false;

    this->modelShader = modelShader;

    this->direct3d = direct3d;
}

Scene::~Scene()
{
    release();

    modelShader.reset();

    direct3d.reset();
}

bool Scene::isInitialized()
{
    return initialized;
}

void Scene::setInitialized()
{
    initialized = true;
    released = false;
}

bool Scene::isReleased()
{
    return released;
}

void Scene::setReleased()
{
    initialized = false;
    released = true;
}

std::vector<std::shared_ptr<Model>> Scene::getModels()
{
    return models;
}

bool Scene::initialize(std::string filename)
{
    if (isInitialized())
    {
        release();
    }

    SceneData sceneData = {};

    bool result = readModels(filename, sceneData);
    if (!result)
    {
        return false;
    }

    result = initializeModels(sceneData);
    if (!result)
    {
        return false;
    }

    setInitialized();
    return true;
}

bool Scene::initialize(SceneData sceneData)
{
    if (isInitialized())
    {
        release();
    }

    bool result = initializeModels(sceneData);
    if (!result)
    {
        return false;
    }

    setInitialized();
    return true;
}

bool Scene::render(DirectX::XMMATRIX vpMatrix)
{
    for (const auto& model : models)
    {
        bool result = model->render(vpMatrix);
        if (!result)
        {
            return false;
        }
    }

    return true;
}

void Scene::release()
{
    if (isReleased())
    {
        return;
    }

    models.clear();
    models.shrink_to_fit();

    setReleased();
}

void Scene::addModel(std::shared_ptr<Model> model)
{
    models.push_back(model);
}

bool Scene::readModels(std::string filename, SceneData& sceneData)
{
    sceneData = {};

    bool result = fileParser.parseFile(filename, sceneData);
    if (!result)
    {
        return false;
    }

    return true;
}

bool Scene::initializeModels(SceneData sceneData)
{
    std::unordered_map<std::string, std::shared_ptr<Model>> uniqueModels;
    uniqueModels.reserve(sceneData.uniqueModelDataItems.size());
    for (const auto& pair : sceneData.uniqueModelDataItems)
    {
        std::shared_ptr<Model>& uniqueModel = uniqueModels[pair.first];
        uniqueModel = createSharedPointer<Model>(modelShader, direct3d);
        bool result = uniqueModel->initialize(pair.second);
        if (!result)
        {
            return false;
        }
    }

    for (const auto& modelData : sceneData.modelDataItems)
    {
        std::shared_ptr<Model>& uniqueModel = uniqueModels[modelData.filename];

        std::shared_ptr<Model> model = copyFromSharedPointer<Model>(uniqueModel);
        model->setTransformation(modelData.transformation);

        models.push_back(model);
    }

    uniqueModels.clear();

    return true;
}
