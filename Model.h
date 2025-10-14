#pragma once
#include <memory>

#include <string>

#include <vector>
#include <unordered_map>

#include "Direct3d.h"

#include "ModelFileParser.h"

#include "VertexBuffer.h"

#include "Mesh.h"
#include "Material.h"

#include "Transformation.h"

#include "ShaderUtility.h"
#include "ConstantBufferUtility.h"

#include "ModelFileParserUtility.h"

class Model
{
    bool initialized;
    bool released;

    std::shared_ptr<Direct3d> direct3d;

    std::shared_ptr<Shader> shader;

    ModelFileParser fileParser;

    std::shared_ptr<VertexBuffer<Vertex>> vertexBuffer;

    std::vector<std::shared_ptr<Mesh>> meshes;

    Transformation transformation;

public:
    Model(std::shared_ptr<Shader> shader, std::shared_ptr<Direct3d> direct3d);
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

    bool initialize(std::string filename,
                            Transformation transformation = Transformation::identity);
    bool initialize(ModelData modelData,
                            Transformation transformation = Transformation::identity);
    bool render(DirectX::XMMATRIX viewProjectionMatrix);
    void release();

private:
    bool readMeshes(std::string filename, ModelData& modelData);
    bool initializeVertexBuffer(ModelData modelData);
    bool initializeMeshes(ModelData modelData);
};
