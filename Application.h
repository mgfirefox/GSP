#pragma once
#include <Windows.h>

#include <memory>

#include "Window.h"
#include "Input.h"
#include "Renderer.h"

const std::string CLOSE_APPLICATION_ACTION_NAME = "closeApplication";

class Application {
    bool initialized;
    bool released;

    LPCWSTR name;
    HINSTANCE instanceHandle;

    ATOM classAtom;

    std::unique_ptr<Window> window;
    std::unique_ptr<Renderer> renderer;

    static std::unique_ptr<Application> application;

    bool shouldShutdown;

public:
    Application();
    ~Application();

private:
    bool isInitialized();
    void setInitialized();

    bool isReleased();
    void setReleased();

public:
    static std::unique_ptr<Application>& getApplication();

    bool initialize(LPCWCHAR name, HINSTANCE instanceHandle, int showCmd = SW_SHOW);
    void run();
    void release();

private:
    static LRESULT CALLBACK windowProcedure(HWND windowHandle, UINT message, WPARAM wparam, LPARAM lparam);
    LRESULT CALLBACK onWindowMessages(HWND windowHandle, UINT message, WPARAM wparam, LPARAM lparam);
    bool renderFrame();
};
