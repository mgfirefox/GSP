#include "Window.h"

Window::Window() : name()
{
    initialized = false;
    released = false;

    fullscreenEnabled = false;

    classAtom = 0;
    applicationInstanceHandle = nullptr;

    handle = nullptr;

    width = 0;
    height = 0;
}

Window::~Window()
{
    release();
}

bool Window::isInitialized()
{
    return initialized;
}

void Window::setInitialized()
{
    initialized = true;
    released = false;
}

bool Window::isReleased()
{
    return released;
}

void Window::setReleased()
{
    initialized = false;
    released = true;
}

bool Window::isFullscreenEnabled()
{
    return fullscreenEnabled;
}

int32 Window::getWidth()
{
    return width;
}

int32 Window::getHeight()
{
    return height;
}

HWND Window::getHandle()
{
    return handle;
}

bool Window::initializeFullscreen(WNDCLASSEX classDesc, HINSTANCE applicationInstanceHandle,
                                  std::wstring name, int32 showCmd, int32 width, int32 height)
{
    if (isInitialized())
    {
        release();
    }

    fullscreenEnabled = true;

    this->width = width;
    this->height = height;

    bool result = initializeClassAtom(classDesc);
    if (!result)
    {
        return false;
    }
    this->applicationInstanceHandle = applicationInstanceHandle;

    DEVMODE displaySettings = {};
    displaySettings.dmSize = sizeof(DEVMODE);
    displaySettings.dmBitsPerPel = 32;
    displaySettings.dmPelsWidth = this->width;
    displaySettings.dmPelsHeight = this->height;
    displaySettings.dmFields = DM_BITSPERPEL | DM_PELSWIDTH | DM_PELSHEIGHT;

    ChangeDisplaySettings(&displaySettings, CDS_FULLSCREEN);

    this->name = std::move(name);

    result = initializeHandle(0, 0, showCmd);
    if (!result)
    {
        return false;
    }

    setInitialized();
    return true;
}

bool Window::initializeWindowed(WNDCLASSEX classDesc, HINSTANCE applicationInstanceHandle,
                                std::wstring name, int32 showCmd, int32 width, int32 height)
{
    if (isInitialized())
    {
        release();
    }

    fullscreenEnabled = false;

    this->width = width;
    this->height = height;

    bool result = initializeClassAtom(classDesc);
    if (!result)
    {
        return false;
    }
    this->applicationInstanceHandle = applicationInstanceHandle;

    this->name = std::move(name);

    int32 fullscreenWidth = GetSystemMetrics(SM_CXSCREEN);
    int32 fullscreenHeight = GetSystemMetrics(SM_CYSCREEN);

    int32 x = (fullscreenWidth - this->width) / 2;
    int32 y = (fullscreenHeight - this->height) / 2;

    result = initializeHandle(x, y, showCmd);
    if (!result)
    {
        return false;
    }

    setInitialized();
    return true;
}

void Window::release()
{
    if (isReleased())
    {
        return;
    }

    ShowCursor(true);
    if (fullscreenEnabled)
    {
        ChangeDisplaySettings(nullptr, 0);
    }
    DestroyWindow(handle);
    handle = nullptr;
    name.clear();
    name.shrink_to_fit();

    UnregisterClass(MAKEINTATOM(classAtom), applicationInstanceHandle);
    classAtom = 0;
    applicationInstanceHandle = nullptr;

    height = 0;
    width = 0;

    fullscreenEnabled = false;

    setReleased();
}

bool Window::initializeClassAtom(WNDCLASSEX classDesc)
{
    classAtom = RegisterClassEx(&classDesc);
    if (classAtom == 0)
    {
        return false;
    }

    return true;
}

bool Window::initializeHandle(int32 x, int32 y, int32 showCmd)
{
    handle = CreateWindowEx(WS_EX_APPWINDOW, MAKEINTATOM(classAtom), name.c_str(),
                            WS_CLIPSIBLINGS | WS_CLIPCHILDREN | WS_POPUP, x, y, width,
                            height, nullptr, nullptr, applicationInstanceHandle, nullptr);
    if (!handle)
    {
        return false;
    }
    ShowWindow(handle, showCmd);
    SetForegroundWindow(handle);
    SetFocus(handle);
    ShowCursor(true);

    return true;
}
