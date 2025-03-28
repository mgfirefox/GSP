#pragma once
#include <d3d11.h>
#include <DirectXMath.h>

#include <wrl/client.h>
#include <memory>

#include <functional>

#include <string>
#include <fstream>
#include <sstream>

#include <vector>
#include <unordered_map>

#include "VertexBuffer.h"
#include "IndexBuffer.h"
#include "Texture2D.h"

#include "Shader.h"
#include "ShaderResource.h"

#include "FileParsers.h"

#include "transformation.h"

const UINT PS_TEXTURE_RESOURCE_SLOT = 0;

class Sprite {
    bool initialized;
    bool released;

    std::unique_ptr<Transformation> transformation;

    std::shared_ptr<VertexBuffer<Vertex>> vertexBuffer;
    std::shared_ptr<IndexBuffer> indexBuffer;

    std::shared_ptr<ShaderResource> textureResource;

public:
    static const std::function<void(Sprite*)> deleter;

    Sprite();
    Sprite(const Sprite& sprite);
    ~Sprite();

private:
    bool isInitialized();
    void setInitialized();

    bool isReleased();
    void setReleased();

public:
    std::unique_ptr<Transformation>& getTransformation();

    bool initialize(Microsoft::WRL::ComPtr<ID3D11Device> device, Microsoft::WRL::ComPtr<ID3D11DeviceContext> deviceContext, std::string filename, DirectX::XMFLOAT2 relativeSize);
    void render(Microsoft::WRL::ComPtr<ID3D11DeviceContext> deviceContext, std::unique_ptr<Shader>& shader);
    void release();
};
