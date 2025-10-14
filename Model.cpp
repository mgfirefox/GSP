#include "Model.h"

Model::Model(std::shared_ptr<Shader> shader, std::shared_ptr<Direct3d> direct3d) : fileParser(),
    vertexBuffer(), meshes()
{
    initialized = false;
    released = false;

    this->shader = shader;

    this->direct3d = direct3d;

    transformation = Transformation::identity;
}

Model::Model(const Model& model)
{
    initialized = model.initialized;
    released = model.released;

    direct3d = model.direct3d;

    shader = model.shader;

    vertexBuffer = model.vertexBuffer;

    meshes = model.meshes;

    transformation = model.transformation;
}

Model::~Model()
{
    Model::release();

    shader.reset();

    direct3d.reset();
}

bool Model::isInitialized()
{
    return initialized;
}

void Model::setInitialized()
{
    initialized = true;
    released = false;
}

bool Model::isReleased()
{
    return released;
}

void Model::setReleased()
{
    initialized = false;
    released = true;
}

Transformation Model::getTransformation()
{
    return transformation;
}

void Model::setTransformation(Transformation transformation)
{
    this->transformation = transformation;
}

bool Model::initialize(std::string filename, Transformation transformation)
{
    if (isInitialized())
    {
        release();
    }

    ModelData modelData = {};

    bool result = readMeshes(filename, modelData);
    if (!result)
    {
        return false;
    }

    result = initializeVertexBuffer(modelData);
    if (!result)
    {
        return false;
    }

    result = initializeMeshes(modelData);
    if (!result)
    {
        return false;
    }

    this->transformation = transformation;

    setInitialized();
    return true;
}

bool Model::initialize(ModelData modelData, Transformation transformation)
{
    if (isInitialized())
    {
        release();
    }

    bool result = initializeVertexBuffer(modelData);
    if (!result)
    {
        return false;
    }

    result = initializeMeshes(modelData);
    if (!result)
    {
        return false;
    }

    this->transformation = transformation;

    setInitialized();
    return true;
}

bool Model::render(DirectX::XMMATRIX viewProjectionMatrix)
{
    bool result = shader->setVertexBuffer(vertexBuffer);
    if (!result)
    {
        return false;
    }

    DirectX::XMMATRIX modelMatrix = transformation.getTransformationMatrix();

    MvpBuffer mvpBuffer = {};
    mvpBuffer.mvpMatrix = DirectX::XMMatrixMultiply(modelMatrix, viewProjectionMatrix);
    mvpBuffer.mvpMatrix = DirectX::XMMatrixTranspose(mvpBuffer.mvpMatrix);

    result = shader->setVertexShaderConstantBufferData(&mvpBuffer, VsMvpBufferSlotIndex);
    if (!result)
    {
        return false;
    }

    for (auto& mesh : meshes)
    {
        result = mesh->render();
        if (!result)
        {
            return false;
        }
    }

    return true;
}

void Model::release()
{
    if (isReleased())
    {
        return;
    }

    transformation = Transformation::identity;

    meshes.clear();

    vertexBuffer.reset();

    setReleased();
}

bool Model::readMeshes(std::string filename, ModelData& modelData)
{
    bool result = fileParser.parseFile(filename, modelData);
    if (!result)
    {
        return false;
    }

    return true;
}

bool Model::initializeVertexBuffer(ModelData modelData)
{
    vertexBuffer = createSharedPointer<VertexBuffer<Vertex>>(direct3d);
    bool result = vertexBuffer->initialize(modelData.vertexes.data(), modelData.vertexes.size());
    if (!result)
    {
        return false;
    }

    return true;
}

bool Model::initializeMeshes(ModelData modelData)
{
    std::unordered_map<std::string, std::shared_ptr<Material>> uniqueMaterials;
    uniqueMaterials.reserve(modelData.materialDataItems.size());
    for (const auto& pair : modelData.materialDataItems)
    {
        std::shared_ptr<Material>& uniqueMaterial = uniqueMaterials[pair.first];
        uniqueMaterial = createSharedPointer<Material>(shader, direct3d);
        bool result = uniqueMaterial->initialize(pair.second);
        if (!result)
        {
            return false;
        }
    }

    for (const auto& meshData : modelData.meshDataItems)
    {
        std::shared_ptr<Material>& uniqueMaterial = uniqueMaterials[meshData.materialName];

        std::shared_ptr<IndexBuffer> indexBuffer = createSharedPointer<IndexBuffer>(direct3d);
        bool result = indexBuffer->initialize(meshData.indexes.data(), meshData.indexes.size());
        if (!result)
        {
            return false;
        }

        std::shared_ptr<Material> material = copyFromSharedPointer<Material>(uniqueMaterial);

        std::shared_ptr<Mesh> mesh = createSharedPointer<Mesh>(shader, direct3d);
        result = mesh->initialize(indexBuffer, material);
        if (!result)
        {
            return false;
        }

        meshes.push_back(mesh);
    }

    uniqueMaterials.clear();

    return true;
}
