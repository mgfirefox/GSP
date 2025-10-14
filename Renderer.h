#pragma once
#include <memory>

#include <string>

#include <vector>
#include <unordered_map>

#include "Input.h"
#include "Timer.h"

#include "Window.h"

#include "Direct3d.h"

#include "Shader.h"
#include "Scene.h"
#include "Camera.h"
#include "Sprite.h"

#include "DirectSound.h"

#include "Sound.h"
#include "Sound3d.h"

#include "Xaudio2.h"

#include "XAudio2Sound.h"

#include "MemoryUtility.h"
#include "ShaderUtility.h"
#include "SoundUtility.h"

class Renderer
{
    bool initialized;
    bool released;

    bool vsyncEnabled;

    std::shared_ptr<Input> input;
    std::shared_ptr<Timer> timer;

    std::shared_ptr<Window> window;

    static const std::wstring materialShaderFilename;
    static const std::wstring textureShaderFilename;

    static const std::string sceneFilename;

    static const std::string spriteTextureFilename;

    static const float perspectiveNear;
    static const float perspectiveFar;

    static const float orthographicNear;
    static const float orthographicFar;

    static const float fieldOfView;

    static const std::string resetCameraActionName;
    static const std::string cameraRotationActionName;

    static const float cameraMovementSpeed;
    static const float keyboardCameraRotationSpeed;
    static const float mouseCameraRotationSpeed;

    static const std::string sound3dFilename;

    std::shared_ptr<Direct3d> direct3d;

    std::shared_ptr<Shader> materialShader;
    std::shared_ptr<Shader> textureShader;

    std::shared_ptr<Scene> scene;

    std::shared_ptr<Camera> camera;

    std::shared_ptr<Sprite> sprite;

    std::shared_ptr<DirectSound> directSound;

    std::shared_ptr<Sound3d> sound3d;

    std::shared_ptr<Xaudio2> xaudio2;

public:
    Renderer(std::shared_ptr<Window> window, std::shared_ptr<Input> input,
             std::shared_ptr<Timer> timer);
    ~Renderer();

private:
    bool isInitialized();
    void setInitialized();

    bool isReleased();
    void setReleased();

public:
    bool initialize(bool isVsyncEnabled);
    bool renderFrame();
    void release();

private:
    bool initializeDirect3d();
    bool initializeShaders();
    bool initializeScene();
    bool initializeCamera();
    bool initializeSprite();
    bool initializeDirectSound();
    bool initializeSounds();
    bool initializeXaudio2();

    bool updateCamera();
    bool updateScene();
    bool updateSprite();
    bool updateListener3d();
    bool updateSounds();
};
