#pragma once
#include <memory>

#include <string>

#include <vector>

#include "Direct3D.h"
#include "Camera.h"
#include "Model.h"
#include "Shader.h"
#include "Input.h"
#include "Scene.h"

const bool FULLSCREEN_ENABLED = false;
const bool VSYNC_ENABLED = true;

const float SCREEN_NEAR = 0.3f;
const float SCREEN_FAR = 1000.0f;

const float FIELD_OF_VIEW = DirectX::XMConvertToRadians(45.0f);

const std::string RESET_CAMERA_ROTATION_ACTION_NAME = "resetCameraRotation";
const std::string CAMERA_ROTATION_ACTION_NAME = "cameraRotation";

const float cameraMovementSpeed = 1.0f;
const float cameraRotationSpeed = 0.5f;

class Renderer {
    bool initialized;
    bool released;

    std::unique_ptr<Direct3D> direct3D;
    std::unique_ptr<Camera> camera;
    std::unique_ptr<Shader> shader;
    std::unique_ptr<Scene> scene;

    DirectX::XMINT3 lastMousePosition;

public:
    Renderer();
    ~Renderer();

private:
    bool isInitialized();
    void setInitialized();

    bool isReleased();
    void setReleased();

public:
    bool initialize(int screenWidth, int screenHeight, HWND windowHandle);
    bool renderFrame();
    void release();
};
