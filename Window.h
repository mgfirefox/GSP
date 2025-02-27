#pragma once
#include <Windows.h>

class Window {
    bool initialized;
    bool released;

    bool fullscreenEnabled;

    int width;
    int height;

    HWND handle;

    ATOM classAtom;

public:
    Window();
    ~Window();

private:
    bool isInitialized();
    void setInitialized();

    bool isReleased();
    void setReleased();

public:
    int getWidth();
    int getHeight();
    HWND getHandle();

    void initialize(ATOM classAtom, LPCWCHAR applicationName, HINSTANCE applicationInstanceHandle, int showCmd = SW_SHOW, bool fullscreenEnabled = false, int width = 800, int height = 600);
    void release();
};
