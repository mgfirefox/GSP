#pragma once
#include <wrl/client.h>
#include <memory>

#include "Direct3d.h"

#include "Shader.h"

#include "IndexBuffer.h"

#include "Material.h"

class Mesh
{
    bool initialized;
    bool released;

    std::shared_ptr<Direct3d> direct3d;

    std::shared_ptr<Shader> shader;

    std::shared_ptr<IndexBuffer> indexBuffer;

    std::shared_ptr<Material> material;

public:
    Mesh(std::shared_ptr<Shader> shader, std::shared_ptr<Direct3d> direct3d);
    ~Mesh();

private:
    bool isInitialized();
    void setInitialized();

    bool isReleased();
    void setReleased();

public:
    bool initialize(std::shared_ptr<IndexBuffer> indexBuffer, std::shared_ptr<Material> material);
    bool render();
    void release();
};
