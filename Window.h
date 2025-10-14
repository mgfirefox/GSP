#pragma once
#include <Windows.h>

#include <string>

#include "IntUtility.h"

class Window
{
    bool initialized;
    bool released;

    bool fullscreenEnabled;

    int32 width;
    int32 height;

    HINSTANCE applicationInstanceHandle;
    ATOM classAtom;

    std::wstring name;
    HWND handle;

public:
    Window();
    ~Window();

private:
    bool isInitialized();
    void setInitialized();

    bool isReleased();
    void setReleased();

public:
    bool isFullscreenEnabled();

    int32 getWidth();
    int32 getHeight();
    HWND getHandle();

    bool initializeFullscreen(WNDCLASSEX classDesc, HINSTANCE applicationInstanceHandle,
                              std::wstring name, int32 showCmd = SW_SHOW,
                              int32 width = GetSystemMetrics(SM_CXSCREEN),
                              int32 height = GetSystemMetrics(SM_CYSCREEN));
    bool initializeWindowed(WNDCLASSEX classDesc, HINSTANCE applicationInstanceHandle,
                            std::wstring name, int32 showCmd = SW_SHOW,
                            int32 width = GetSystemMetrics(SM_CXSCREEN),
                            int32 height = GetSystemMetrics(SM_CYSCREEN));
    void release();

private:
    bool initializeClassAtom(WNDCLASSEX classDesc);
    bool initializeHandle(int32 x, int32 y, int32 showCmd);
};
