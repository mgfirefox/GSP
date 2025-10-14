#pragma once
#include <DirectXMath.h>

struct Transformation
{
    static const DirectX::XMFLOAT3 worldForward;
    static const DirectX::XMFLOAT3 worldUp;
    static const DirectX::XMFLOAT3 worldRight;

    static const Transformation identity;

    DirectX::XMFLOAT3 position;
    DirectX::XMFLOAT3 orientation;
    DirectX::XMFLOAT3 scale_;

    Transformation();
    Transformation(DirectX::XMFLOAT3 position, DirectX::XMFLOAT3 orientation,
                   DirectX::XMFLOAT3 scale);

    DirectX::XMVECTOR getPosition();
    void setPosition(DirectX::XMVECTOR position);

    DirectX::XMVECTOR getOrientation();
    void setOrientation(DirectX::XMVECTOR rotation);

    DirectX::XMVECTOR getScale();
    void setScale(DirectX::XMVECTOR scaling);

    DirectX::XMVECTOR getForward();
    DirectX::XMVECTOR getUp();
    DirectX::XMVECTOR getRight();

    DirectX::XMMATRIX getTranslationMatrix();
    DirectX::XMMATRIX getRotationMatrix();
    DirectX::XMMATRIX getScalingMatrix();

    DirectX::XMMATRIX getTransformationMatrix();

    void translate(DirectX::XMFLOAT3 translation, bool isLocally = false);
    void rotate(DirectX::XMFLOAT3 rotation);
    void scale(DirectX::XMFLOAT3 scaling);
};
