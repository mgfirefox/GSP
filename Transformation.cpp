#include "Transformation.h"

const DirectX::XMFLOAT3 Transformation::worldForward(0.0f, 0.0f, 1.0f);
const DirectX::XMFLOAT3 Transformation::worldUp(0.0f, 1.0f, 0.0f);
const DirectX::XMFLOAT3 Transformation::worldRight(1.0f, 0.0f, 0.0f);

const Transformation Transformation::identity({}, {}, {1.0f, 1.0f, 1.0f});

Transformation::Transformation() : position(), orientation(), scale_()
{
}

Transformation::Transformation(DirectX::XMFLOAT3 position, DirectX::XMFLOAT3 orientation,
                               DirectX::XMFLOAT3 scale): position(position),
                                                         orientation(orientation),
                                                         scale_(scale)
{
}

DirectX::XMVECTOR Transformation::getPosition()
{
    return DirectX::XMLoadFloat3(&position);
}

void Transformation::setPosition(DirectX::XMVECTOR position)
{
    DirectX::XMStoreFloat3(&this->position, position);
}

DirectX::XMVECTOR Transformation::getOrientation()
{
    return DirectX::XMLoadFloat3(&orientation);
}

void Transformation::setOrientation(DirectX::XMVECTOR rotation)
{
    DirectX::XMStoreFloat3(&this->orientation, rotation);
}

DirectX::XMVECTOR Transformation::getScale()
{
    return DirectX::XMLoadFloat3(&scale_);
}

void Transformation::setScale(DirectX::XMVECTOR scaling)
{
    DirectX::XMStoreFloat3(&this->scale_, scaling);
}

DirectX::XMVECTOR Transformation::getForward()
{
    DirectX::XMMATRIX rotationMatrix = getRotationMatrix();
    DirectX::XMVECTOR forward = DirectX::XMLoadFloat3(&worldForward);
    forward = DirectX::XMVector3TransformCoord(forward, rotationMatrix);

    return DirectX::XMVector3Normalize(forward);
}

DirectX::XMVECTOR Transformation::getUp()
{
    DirectX::XMMATRIX rotationMatrix = getRotationMatrix();
    DirectX::XMVECTOR up = DirectX::XMLoadFloat3(&worldUp);
    up = DirectX::XMVector3TransformCoord(up, rotationMatrix);

    return DirectX::XMVector3Normalize(up);
}

DirectX::XMVECTOR Transformation::getRight()
{
    DirectX::XMMATRIX rotationMatrix = getRotationMatrix();
    DirectX::XMVECTOR right = DirectX::XMLoadFloat3(&worldRight);
    right = DirectX::XMVector3TransformCoord(right, rotationMatrix);

    return DirectX::XMVector3Normalize(right);
}

DirectX::XMMATRIX Transformation::getTranslationMatrix()
{
    return DirectX::XMMatrixTranslation(position.x, position.y, position.z);
}

DirectX::XMMATRIX Transformation::getRotationMatrix()
{
    float pitch = DirectX::XMConvertToRadians(orientation.x);
    float yaw = DirectX::XMConvertToRadians(orientation.y);
    float roll = DirectX::XMConvertToRadians(orientation.z);

    return DirectX::XMMatrixRotationRollPitchYaw(pitch, yaw, roll);
}

DirectX::XMMATRIX Transformation::getScalingMatrix()
{
    return DirectX::XMMatrixScaling(scale_.x, scale_.y, scale_.z);
}

DirectX::XMMATRIX Transformation::getTransformationMatrix()
{
    DirectX::XMMATRIX scaleMatrix = getScalingMatrix();
    DirectX::XMMATRIX rotateMatrix = getRotationMatrix();
    DirectX::XMMATRIX translateMatrix = getTranslationMatrix();

    DirectX::XMMATRIX scaleRotateMatrix = DirectX::XMMatrixMultiply(scaleMatrix, rotateMatrix);
    return DirectX::XMMatrixMultiply(scaleRotateMatrix, translateMatrix);
}

void Transformation::translate(DirectX::XMFLOAT3 translation, bool isLocally)
{
    if (isLocally)
    {
        DirectX::XMVECTOR forward = getForward();
        DirectX::XMVECTOR right = getRight();
        DirectX::XMVECTOR up = getUp();

        DirectX::XMMATRIX forwardScaleMatrix = DirectX::XMMatrixScaling(
            translation.z, translation.z, translation.z);
        forward = DirectX::XMVector3TransformCoord(forward, forwardScaleMatrix);

        DirectX::XMMATRIX rightScaleMatrix = DirectX::XMMatrixScaling(
            translation.x, translation.x, translation.x);
        right = DirectX::XMVector3TransformCoord(right, rightScaleMatrix);

        DirectX::XMMATRIX upScaleMatrix = DirectX::XMMatrixScaling(
            translation.y, translation.y, translation.y);
        up = DirectX::XMVector3TransformCoord(up, upScaleMatrix);

        DirectX::XMVECTOR direction = DirectX::XMVectorAdd(forward, right);
        direction = DirectX::XMVectorAdd(direction, up);

        DirectX::XMVECTOR position = getPosition();
        position = DirectX::XMVectorAdd(position, direction);
        setPosition(position);
    }
    else
    {
        position.x += translation.x;
        position.y += translation.y;
        position.z += translation.z;
    }
}

void Transformation::rotate(DirectX::XMFLOAT3 rotation)
{
    this->orientation.x += rotation.x;
    this->orientation.y += rotation.y;
    this->orientation.z += rotation.z;
}

void Transformation::scale(DirectX::XMFLOAT3 scaling)
{
    this->scale_.x *= scaling.x;
    this->scale_.y *= scaling.y;
    this->scale_.z *= scaling.z;
}
