#pragma once
#include <Windows.h>

#include <memory>

#include <string>

#include "Input.h"
#include "Timer.h"
#include "FpsCounter.h"

#include "Window.h"
#include "Renderer.h"

#include "MemoryUtility.h"

class Application
{
    bool initialized;
    bool released;

    std::shared_ptr<Input> input;
    std::shared_ptr<Timer> timer;
    std::shared_ptr<FpsCounter> fpsCounter;

    static const bool fullscreenEnabled;

    static const bool vsyncEnabled;

    static const uint32 fpsLimit;

    static const int32 windowWidth;
    static const int32 windowHeight;

    static const std::string closeApplicationActionName;

    static std::shared_ptr<Application> application;

    HINSTANCE instanceHandle;
    std::wstring name;

    std::shared_ptr<Window> window;
    std::shared_ptr<Renderer> renderer;

    HRESULT comInitializationResult;

    bool shouldShutdown;

public:
    Application(std::shared_ptr<Input> input, std::shared_ptr<Timer> timer,
                std::shared_ptr<FpsCounter> fpsCounter);
    ~Application();

private:
    bool isInitialized();
    void setInitialized();

    bool isReleased();
    void setReleased();

public:
    bool initialize(HINSTANCE instanceHandle, std::wstring name,
                    std::shared_ptr<Application> application, int32
                    showCmd = SW_SHOW);
    void run();
    void release();

private:
    bool initializeWindow(int32 showCmd);
    bool initializeRenderer();
    bool bindActions();
    bool initializeCom();

    bool sendWindowsMessages();

    static LRESULT CALLBACK windowProcedure(HWND windowHandle, UINT message, WPARAM wparam,
                                            LPARAM lparam);
    LRESULT CALLBACK onAllWindowMessages(HWND windowHandle, UINT message, WPARAM wparam,
                                         LPARAM lparam);
};
