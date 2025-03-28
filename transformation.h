#pragma once
#include <DirectXMath.h>

const DirectX::XMFLOAT3 WORLD_FORWARD = DirectX::XMFLOAT3(0.0f, 0.0f, 1.0f);
const DirectX::XMFLOAT3 WORLD_UP = DirectX::XMFLOAT3(0.0f, 1.0f, 0.0f);
const DirectX::XMFLOAT3 WORLD_RIGHT = DirectX::XMFLOAT3(1.0f, 0.0f, 0.0f);

struct Transformation {
    DirectX::XMFLOAT3 position;
    DirectX::XMFLOAT3 rotation;
    DirectX::XMFLOAT3 scaling;

    Transformation();

    DirectX::XMVECTOR getPosition();
    void setPosition(DirectX::XMVECTOR position);

    DirectX::XMVECTOR getRotation();
    void setRotation(DirectX::XMVECTOR rotation);

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
