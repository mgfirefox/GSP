#pragma once
#include <d3d11.h>

#include <wrl/client.h>
#include <memory>

#include <functional>

#include <string>
#include <fstream>
#include <sstream>

#include <vector>
#include <unordered_map>

#include "VertexBuffer.h"

#include "Mesh.h"
#include "Material.h"

#include "FileParsers.h"

#include "transformation.h"

class Model {
    bool initialized;
    bool released;

    std::unique_ptr<Transformation> transformation;

    std::shared_ptr<VertexBuffer<Vertex>> vertexBuffer;

    std::vector<std::shared_ptr<Mesh>> meshes;

public:
    static const std::function<void(Model*)> deleter;

    Model();
    Model(const Model& model);
    ~Model();

private:
    bool isInitialized();
    void setInitialized();

    bool isReleased();
    void setReleased();

public:
    std::unique_ptr<Transformation>& getTransformation();
    void setTransformation(Transformation transformation);

    bool initialize(Microsoft::WRL::ComPtr<ID3D11Device> device, Microsoft::WRL::ComPtr<ID3D11DeviceContext> deviceContext, std::string filename, Transformation transformation = {});
    bool initialize(Microsoft::WRL::ComPtr<ID3D11Device> device, Microsoft::WRL::ComPtr<ID3D11DeviceContext> deviceContext, ModelData modelData, Transformation transformation = {});
    bool render(Microsoft::WRL::ComPtr<ID3D11DeviceContext> deviceContext, std::unique_ptr<Shader>& shader);
    void release();
};
