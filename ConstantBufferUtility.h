#pragma once
#include <DirectXMath.h>

struct MvpBuffer
{
    DirectX::XMMATRIX mvpMatrix;
};

struct MaterialBuffer
{
    DirectX::XMFLOAT3 diffuseColor;
    float opacity;

    bool hasDiffuseColorTexture;
};
