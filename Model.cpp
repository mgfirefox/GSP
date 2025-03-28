#include "Model.h"

const std::function<void(Model*)> Model::deleter = [](Model* model) {
    model->release();
    delete model;
    };

Model::Model() : transformation(), vertexBuffer(), meshes() {
    initialized = false;
    released = false;
}

Model::Model(const Model& model) {
    initialized = model.initialized;
    released = model.released;

    transformation = std::make_unique<Transformation>(*model.transformation);
    
    vertexBuffer = model.vertexBuffer;

    meshes = model.meshes;
}

Model::~Model() {
    release();
}

bool Model::isInitialized() {
    return initialized;
}

void Model::setInitialized() {
    initialized = true;
    released = false;
}

bool Model::isReleased() {
    return released;
}

void Model::setReleased() {
    initialized = false;
    released = true;
}

std::unique_ptr<Transformation>& Model::getTransformation() {
    return transformation;
}

void Model::setTransformation(Transformation transformation) {
    this->transformation->position = transformation.position;
    this->transformation->rotation = transformation.rotation;
    this->transformation->scaling = transformation.scaling;
}

bool Model::initialize(Microsoft::WRL::ComPtr<ID3D11Device> device, Microsoft::WRL::ComPtr<ID3D11DeviceContext> deviceContext, std::string filename, Transformation transformation) {
    if (isInitialized()) {
        release();
    }

    ModelData modelData = {};

    ModelFileParser modelFileParser;
    bool result = modelFileParser.parseFile(filename, modelData);
    if (!result) {
        return false;
    }

    result = initialize(device, deviceContext, modelData, transformation);
    if (!result) {
        return false;
    }

    setInitialized();
    return true;
}

bool Model::initialize(Microsoft::WRL::ComPtr<ID3D11Device> device, Microsoft::WRL::ComPtr<ID3D11DeviceContext> deviceContext, ModelData modelData, Transformation transformation) {
    if (isInitialized()) {
        release();
    }

    vertexBuffer = std::shared_ptr<VertexBuffer<Vertex>>(new VertexBuffer<Vertex>, VertexBuffer<Vertex>::deleter);
    bool result = vertexBuffer->initialize(device, modelData.vertexes.data(), (UINT)modelData.vertexes.size());
    if (!result) {
        return false;
    }

    std::unordered_map<std::string, std::unique_ptr<Material>> uniqueMaterials;
    for (const auto& pair : modelData.materialsDataItems) {
        std::unique_ptr<Material>& material = uniqueMaterials[pair.first];
        material = std::make_unique<Material>();
        result = material->initialize(device, deviceContext, pair.second);
        if (!result) {
            return false;
        }
    }

    for (const auto& meshData : modelData.meshDataItems) {
        std::unique_ptr<Material>& uniqueMaterial = uniqueMaterials[meshData.materialName];
        if (!uniqueMaterial) {
            continue;
        }

        std::shared_ptr<Material> material(new Material(*uniqueMaterial), Material::deleter);

        std::shared_ptr<IndexBuffer> indexBuffer(new IndexBuffer, IndexBuffer::deleter);
        result = indexBuffer->initialize(device, meshData.indexes.data(), (UINT)meshData.indexes.size());
        if (!result) {
            return false;
        }

        std::shared_ptr<Mesh> mesh(new Mesh, Mesh::deleter);
        mesh->initialize(indexBuffer, material);
        meshes.push_back(mesh);
    }

    this->transformation = std::make_unique<Transformation>(transformation);

    for (auto& pair : uniqueMaterials) {
        pair.second->release();
        pair.second.reset();
    }
    uniqueMaterials.clear();

    setInitialized();
    return true;
}

bool Model::render(Microsoft::WRL::ComPtr<ID3D11DeviceContext> deviceContext, std::unique_ptr<Shader>& shader) {
    vertexBuffer->set(deviceContext);

    for (auto& mesh : meshes) {
        bool result = mesh->render(deviceContext, shader);
        if (!result) {
            return false;
        }
    }

    return true;
}

void Model::release() {
    if (isReleased()) {
        return;
    }

    for (auto& mesh : meshes) {
        mesh.reset();
    }
    meshes.clear();

    vertexBuffer.reset();

    transformation.reset();

    setReleased();
}
