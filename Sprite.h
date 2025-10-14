#pragma once
#define NOMINMAX

#include <d3d11.h>
#include <DirectXMath.h>

#include <wrl/client.h>
#include <memory>

#include <string>
#include <fstream>
#include <sstream>

#include <vector>
#include <array>
#include <unordered_map>

#include "Direct3d.h"

#include "Shader.h"

#include "VertexBuffer.h"
#include "IndexBuffer.h"

#include "Material.h"

#include "Vertex.h"

#include "IntUtility.h"

#include "ShaderUtility.h"

#include "ImageFileParserUtility.h"

#include "Transformation.h"

class Sprite
{
    bool initialized;
    bool released;

    std::shared_ptr<Direct3d> direct3d;

    std::shared_ptr<Shader> shader;

    std::shared_ptr<Window> window;

    std::shared_ptr<VertexBuffer<Vertex>> vertexBuffer;
    std::shared_ptr<IndexBuffer> indexBuffer;

    std::shared_ptr<Texture> diffuseColorTexture;

    Transformation transformation;

public:
    Sprite(std::shared_ptr<Shader> shader, std::shared_ptr<Window> window,
           std::shared_ptr<Direct3d> direct3d);
    Sprite(const Sprite& sprite);
    ~Sprite();

private:
    bool isInitialized();
    void setInitialized();

    bool isReleased();
    void setReleased();

public:
    Transformation getTransformation();

    bool initialize(std::string filename, DirectX::XMFLOAT2 relativeSize);
    bool render(DirectX::XMMATRIX projectionMatrix);
    void release();
};
