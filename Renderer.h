#pragma once
#include <memory>

#include <string>

#include <vector>

#include "Direct3D.h"
#include "Shader.h"
#include "Scene.h"
#include "Camera.h"
#include "Timer.h"
#include "Input.h"
#include "Sprite.h"

const bool FULLSCREEN_ENABLED = false;
const bool VSYNC_ENABLED = true;
//const int FPS_LIMIT = 480;

const float PERSPECTIVE_NEAR = 0.1f;
const float PERSPECTIVE_FAR = 1000.0f;

const float ORTHOGRAPHIC_NEAR = 0.1f;
const float ORTHOGRAPHIC_FAR = 100.0f;

const float FIELD_OF_VIEW = DirectX::XMConvertToRadians(45.0f);

const std::string RESET_CAMERA_ACTION_NAME = "resetCamera";
const std::string CAMERA_ROTATION_ACTION_NAME = "cameraRotation";

const UINT VS_MVP_BUFFER_SLOT = 0;

const UINT PS_SAMPLER_SLOT = 0;

const float cameraMovementSpeed = 50.0f;
const float cameraRotationSpeed = 10.0f;

class Renderer {
    bool initialized;
    bool released;

    std::unique_ptr<Direct3D> direct3D;

    std::unique_ptr<Shader> materialShader;
    std::unique_ptr<Shader> textureShader;

    std::unique_ptr<Scene> scene;

    std::unique_ptr<Camera> camera;

    std::shared_ptr<Sprite> sprite;

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
    void updateCamera();
    bool renderFrame();
    void release();
};
