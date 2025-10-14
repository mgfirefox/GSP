#include "Mesh.h"

Mesh::Mesh(std::shared_ptr<Shader> shader, std::shared_ptr<Direct3d> direct3d) : indexBuffer(),
    material()
{
    initialized = false;
    released = false;

    this->shader = shader;

    this->direct3d = direct3d;
}

Mesh::~Mesh()
{
    release();

    shader.reset();

    direct3d.reset();
}

bool Mesh::isInitialized()
{
    return initialized;
}

void Mesh::setInitialized()
{
    initialized = true;
    released = false;
}

bool Mesh::isReleased()
{
    return released;
}

void Mesh::setReleased()
{
    initialized = false;
    released = true;
}

bool Mesh::initialize(std::shared_ptr<IndexBuffer> indexBuffer, std::shared_ptr<Material> material)
{
    if (isInitialized())
    {
        release();
    }

    if (!indexBuffer || !material)
    {
        return false;
    }

    this->indexBuffer = indexBuffer;

    this->material = material;

    setInitialized();
    return true;
}

bool Mesh::render()
{
    bool result = shader->setIndexBuffer(indexBuffer);
    if (!result)
    {
        return false;
    }

    if (material)
    {
        result = material->render();
        if (!result)
        {
            return false;
        }
    }

    result = shader->render();
    if (!result)
    {
        return false;
    }

    return true;
}

void Mesh::release()
{
    if (isReleased())
    {
        return;
    }

    material.reset();

    indexBuffer.reset();

    setReleased();
}
