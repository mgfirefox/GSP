#pragma once
#include <memory>
#include <wrl/client.h>

#include <functional>

#include "IndexBuffer.h"

#include "Material.h"

class Mesh {
    bool initialized;
    bool released;

    std::shared_ptr<IndexBuffer> indexBuffer;

    std::shared_ptr<Material> material;

public:
    static const std::function<void(Mesh*)> deleter;

    Mesh();
    ~Mesh();

private:
    bool isInitialized();
    void setInitialized();

    bool isReleased();
    void setReleased();

public:
    void initialize(std::shared_ptr<IndexBuffer> indexBuffer, std::shared_ptr<Material> material);
    bool render(Microsoft::WRL::ComPtr<ID3D11DeviceContext> deviceContext, std::unique_ptr<Shader>& shader);
    void release();
};
