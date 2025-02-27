#include "transformation.h"

Transformation::Transformation() : position{}, rotation{}, scaling{ 1.0f, 1.0f, 1.0f } {
}

DirectX::XMVECTOR Transformation::getPosition() {
    return DirectX::XMLoadFloat3(&position);
}

void Transformation::setPosition(DirectX::XMVECTOR position) {
    DirectX::XMStoreFloat3(&this->position, position);
}

DirectX::XMVECTOR Transformation::getRotation() {
    return DirectX::XMLoadFloat3(&rotation);
}

void Transformation::setRotation(DirectX::XMVECTOR rotation) {
    DirectX::XMStoreFloat3(&this->rotation, rotation);
}

DirectX::XMVECTOR Transformation::getScale() {
    return DirectX::XMLoadFloat3(&scaling);
}

void Transformation::setScale(DirectX::XMVECTOR scaling) {
    DirectX::XMStoreFloat3(&this->scaling, scaling);
}

DirectX::XMVECTOR Transformation::getForward() {
    DirectX::XMMATRIX rotationMatrix = getRotationMatrix();
    DirectX::XMVECTOR forward = DirectX::XMLoadFloat3(&WORLD_FORWARD);
    forward = DirectX::XMVector3TransformCoord(forward, rotationMatrix);

    return DirectX::XMVector3Normalize(forward);
}

DirectX::XMVECTOR Transformation::getUp() {
    DirectX::XMMATRIX rotationMatrix = getRotationMatrix();
    DirectX::XMVECTOR up = DirectX::XMLoadFloat3(&WORLD_UP);
    up = DirectX::XMVector3TransformCoord(up, rotationMatrix);

    return DirectX::XMVector3Normalize(up);
}

DirectX::XMVECTOR Transformation::getRight() {
    DirectX::XMMATRIX rotationMatrix = getRotationMatrix();
    DirectX::XMVECTOR right = DirectX::XMLoadFloat3(&WORLD_RIGHT);
    right = DirectX::XMVector3TransformCoord(right, rotationMatrix);

    return DirectX::XMVector3Normalize(right);
}

DirectX::XMMATRIX Transformation::getTranslationMatrix() {
    return DirectX::XMMatrixTranslation(position.x, position.y, position.z);
}

DirectX::XMMATRIX Transformation::getRotationMatrix() {
    float pitch = DirectX::XMConvertToRadians(rotation.x);
    float yaw = DirectX::XMConvertToRadians(rotation.y);
    float roll = DirectX::XMConvertToRadians(rotation.z);

    return DirectX::XMMatrixRotationRollPitchYaw(pitch, yaw, roll);
}

DirectX::XMMATRIX Transformation::getScalingMatrix() {
    return DirectX::XMMatrixScaling(scaling.x, scaling.y, scaling.z);
}

DirectX::XMMATRIX Transformation::getTransformationMatrix() {
    DirectX::XMMATRIX scaleMatrix = getScalingMatrix();
    DirectX::XMMATRIX rotateMatrix = getRotationMatrix();
    DirectX::XMMATRIX translateMatrix = getTranslationMatrix();

    DirectX::XMMATRIX scaleRotateMatrix = DirectX::XMMatrixMultiply(scaleMatrix, rotateMatrix);
    return DirectX::XMMatrixMultiply(scaleRotateMatrix, translateMatrix);
}

void Transformation::translate(DirectX::XMFLOAT3 translation, bool isLocally) {
    if (isLocally) {
        DirectX::XMVECTOR forward = getForward();
        DirectX::XMVECTOR right = getRight();
        DirectX::XMVECTOR up = getUp();

        DirectX::XMMATRIX forwardScalingMatrix = DirectX::XMMatrixScaling(translation.z, translation.z, translation.z);
        forward = DirectX::XMVector3TransformCoord(forward, forwardScalingMatrix);

        DirectX::XMMATRIX rightScalingMatrix = DirectX::XMMatrixScaling(translation.x, translation.x, translation.x);
        right = DirectX::XMVector3TransformCoord(right, rightScalingMatrix);

        DirectX::XMMATRIX upScalingMatrix = DirectX::XMMatrixScaling(translation.y, translation.y, translation.y);
        up = DirectX::XMVector3TransformCoord(up, upScalingMatrix);

        DirectX::XMVECTOR direction = DirectX::XMVectorAdd(forward, right);
        direction = DirectX::XMVectorAdd(direction, up);

        DirectX::XMVECTOR position = getPosition();
        position = DirectX::XMVectorAdd(position, direction);
        setPosition(position);
    }
    else {
        position.x += translation.x;
        position.y += translation.y;
        position.z += translation.z;
    }
}

void Transformation::rotate(DirectX::XMFLOAT3 rotation) {
    this->rotation.x += rotation.x;
    this->rotation.y += rotation.y;
    this->rotation.z += rotation.z;
}

void Transformation::scale(DirectX::XMFLOAT3 scaling) {
    this->scaling.x *= scaling.x;
    this->scaling.y *= scaling.y;
    this->scaling.z *= scaling.z;
}
