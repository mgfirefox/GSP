#include "Camera.h"

Camera::Camera() : transformation(), viewMatrix{}, perspectiveProjectionMatrix{}, orthographicProjectionMatrix{} {
    initialized = false;
    released = false;
}

Camera::~Camera() {
    release();
}

bool Camera::isInitialized() {
    return initialized;
}

void Camera::setInitialized() {
    initialized = true;
    released = false;
}

bool Camera::isReleased() {
    return released;
}

void Camera::setReleased() {
    initialized = false;
    released = true;
}

std::unique_ptr<Transformation>& Camera::getTransformation() {
    return transformation;
}

void Camera::setTransformation(Transformation transformation) {
    this->transformation->position = transformation.position;
    this->transformation->rotation = transformation.rotation;
    this->transformation->scaling = transformation.scaling;
}

DirectX::XMMATRIX Camera::getViewMatrix() {
    return DirectX::XMLoadFloat4x4(&viewMatrix);
}

DirectX::XMMATRIX Camera::getPerspectiveProjectionMatrix() {
    return DirectX::XMLoadFloat4x4(&perspectiveProjectionMatrix);
}

DirectX::XMMATRIX Camera::getOrthographicProjectionMatrix() {
    return DirectX::XMLoadFloat4x4(&orthographicProjectionMatrix);
}

bool Camera::initialize(float fieldOfView, float screenWidth, float screenHeight, float screenNear, float screenFar) {
    if (isInitialized()) {
        release();
    }

    transformation = std::make_unique<Transformation>();

    update();

    float aspectRatio = screenWidth / screenHeight;

    DirectX::XMMATRIX perspectiveProjectionMatrix = DirectX::XMMatrixPerspectiveFovLH(fieldOfView, aspectRatio, screenNear, screenFar);
    DirectX::XMStoreFloat4x4(&this->perspectiveProjectionMatrix, perspectiveProjectionMatrix);

    DirectX::XMMATRIX orthographicProjectionMatrix = DirectX::XMMatrixOrthographicLH(screenWidth, screenHeight, screenNear, screenFar);
    DirectX::XMStoreFloat4x4(&this->orthographicProjectionMatrix, orthographicProjectionMatrix);

    setInitialized();
    return true;
}

void Camera::update() {
    DirectX::XMVECTOR position = transformation->getPosition();

    DirectX::XMVECTOR forward = transformation->getForward();
    DirectX::XMVECTOR up = transformation->getUp();

    DirectX::XMVECTOR focusPosition = DirectX::XMVectorAdd(position, forward);

    DirectX::XMMATRIX viewMatrix = DirectX::XMMatrixLookAtLH(position, focusPosition, up);
    DirectX::XMStoreFloat4x4(&this->viewMatrix, viewMatrix);
}

void Camera::release() {
    if (isReleased()) {
        return;
    }

    orthographicProjectionMatrix = {};
    perspectiveProjectionMatrix = {};
    viewMatrix = {};

    transformation.reset();

    setReleased();
}
