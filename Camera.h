#pragma once
#include <DirectXMath.h>

#include "transformation.h"

class Camera {
    bool initialized;
    bool released;

    Transformation transformation;

    DirectX::XMFLOAT4X4 viewMatrix;
    DirectX::XMFLOAT4X4 perspectiveProjectionMatrix;
    DirectX::XMFLOAT4X4 orthographicProjectionMatrix;

public:
    Camera();
    ~Camera();

private:
    bool isInitialized();
    void setInitialized();

    bool isReleased();
    void setReleased();

public:
    Transformation getTransformation();
    void setTransformation(Transformation transformation);

    DirectX::XMMATRIX getViewMatrix();
    DirectX::XMMATRIX getPerspectiveProjectionMatrix();
    DirectX::XMMATRIX getOrthographicProjectionMatrix();

    bool initialize(float fieldOfView, float screenWidth, float screenHeight, float screenNear, float screenFar, Transformation transformation = {});
    void update();
    void release();
};
