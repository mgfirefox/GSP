#include "InputLayout.h"

InputLayout::InputLayout(std::shared_ptr<Direct3d> direct3d) : inputLayout()
{
    initialized = false;
    released = false;

    this->direct3d = direct3d;
}

InputLayout::~InputLayout()
{
    release();

    direct3d.reset();
}

bool InputLayout::isInitialized()
{
    return initialized;
}

void InputLayout::setInitialized()
{
    initialized = true;
    released = false;
}

bool InputLayout::isReleased()
{
    return released;
}

void InputLayout::setReleased()
{
    initialized = false;
    released = true;
}

Microsoft::WRL::ComPtr<ID3D11InputLayout> InputLayout::getInputLayout()
{
    return inputLayout;
}

bool InputLayout::initialize(const D3D11_INPUT_ELEMENT_DESC* inputElementDescs,
                             uint32 inputElementDescCount,
                             Microsoft::WRL::ComPtr<ID3D10Blob> vertexShaderBuffer)
{
    if (isInitialized())
    {
        release();
    }

    bool result = direct3d->createInputLayout(inputLayout, inputElementDescs, inputElementDescCount,
                                              vertexShaderBuffer);
    if (!result)
    {
        return false;
    }

    setInitialized();
    return true;
}

void InputLayout::release()
{
    if (isReleased())
    {
        return;
    }

    inputLayout.Reset();

    setReleased();
}
