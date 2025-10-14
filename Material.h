#pragma once
#include <d3d11.h>

#include <wrl/client.h>
#include <memory>

#include <string>

#include "Direct3d.h"

#include "Shader.h"

#include "Texture.h"

#include "ShaderUtility.h"
#include "ConstantBufferUtility.h"

#include "ModelFileParserUtility.h"

class Material
{
    bool initialized;
    bool released;

    std::shared_ptr<Direct3d> direct3d;

    std::shared_ptr<Shader> shader;

    DirectX::XMFLOAT3 diffuseColor;
    float opacity;

    bool hasDiffuseColorTexture;
    std::shared_ptr<Texture> diffuseColorTexture;

public:
    Material(std::shared_ptr<Shader> shader, std::shared_ptr<Direct3d> direct3d);
    Material(const Material& material);
    ~Material();

private:
    bool isInitialized();
    void setInitialized();

    bool isReleased();
    void setReleased();

public:
    bool initialize(MaterialData materialData);
    bool render();
    void release();

private:
    bool initializeData(MaterialData materialData);
};
