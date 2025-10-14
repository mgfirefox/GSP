#include "Camera.h"

Camera::Camera() : transformation(), viewMatrix{}, perspectiveProjectionMatrix{},
                   orthographicProjectionMatrix{}
{
    initialized = false;
    released = false;

    transformation = Transformation::identity;
}

Camera::~Camera()
{
    release();
}

bool Camera::isInitialized()
{
    return initialized;
}

void Camera::setInitialized()
{
    initialized = true;
    released = false;
}

bool Camera::isReleased()
{
    return released;
}

void Camera::setReleased()
{
    initialized = false;
    released = true;
}

Transformation Camera::getTransformation()
{
    return transformation;
}

void Camera::setTransformation(Transformation transformation)
{
    this->transformation = transformation;

    update();
}

DirectX::XMMATRIX Camera::getViewMatrix()
{
    return DirectX::XMLoadFloat4x4(&viewMatrix);
}

DirectX::XMMATRIX Camera::getPerspectiveProjectionMatrix()
{
    return DirectX::XMLoadFloat4x4(&perspectiveProjectionMatrix);
}

DirectX::XMMATRIX Camera::getOrthographicProjectionMatrix()
{
    return DirectX::XMLoadFloat4x4(&orthographicProjectionMatrix);
}

bool Camera::initialize(float fieldOfView, float windowWidth, float windowHeight, float near,
                        float far, Transformation transformation)
{
    if (isInitialized())
    {
        release();
    }

    float aspectRatio = windowWidth / windowHeight;

    DirectX::XMMATRIX perspectiveProjectionMatrix = DirectX::XMMatrixPerspectiveFovLH(
        fieldOfView, aspectRatio, near, far);
    DirectX::XMStoreFloat4x4(&this->perspectiveProjectionMatrix, perspectiveProjectionMatrix);

    DirectX::XMMATRIX orthographicProjectionMatrix = DirectX::XMMatrixOrthographicLH(
        windowWidth, windowHeight, near, far);
    DirectX::XMStoreFloat4x4(&this->orthographicProjectionMatrix, orthographicProjectionMatrix);

    this->transformation = transformation;

    update();

    setInitialized();
    return true;
}

void Camera::release()
{
    if (isReleased())
    {
        return;
    }

    transformation = Transformation::identity;

    orthographicProjectionMatrix = {};
    perspectiveProjectionMatrix = {};
    viewMatrix = {};

    setReleased();
}

void Camera::update()
{
    DirectX::XMVECTOR position = transformation.getPosition();

    DirectX::XMVECTOR forward = transformation.getForward();
    DirectX::XMVECTOR up = transformation.getUp();

    DirectX::XMVECTOR focusPosition = DirectX::XMVectorAdd(position, forward);

    DirectX::XMMATRIX viewMatrix = DirectX::XMMatrixLookAtLH(position, focusPosition, up);
    DirectX::XMStoreFloat4x4(&this->viewMatrix, viewMatrix);
}
