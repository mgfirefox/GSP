#include "Window.h"

Window::Window() {
    initialized = false;
    released = false;

    fullscreenEnabled = false;

    width = 0;
    height = 0;
    handle = NULL;

    classAtom = 0;
}

Window::~Window() {
    release();
}

bool Window::isInitialized() {
    return initialized;
}

void Window::setInitialized() {
    initialized = true;
    released = false;
}

bool Window::isReleased() {
    return released;
}

void Window::setReleased() {
    initialized = false;
    released = true;
}

int Window::getWidth() {
    return width;
}

int Window::getHeight() {
    return height;
}

HWND Window::getHandle() {
    return handle;
}

void Window::initialize(ATOM classAtom, LPCWCHAR applicationName, HINSTANCE applicationInstanceHandle, int showCmd, bool fullscreenEnabled, int width, int height) {
    if (isInitialized()) {
        release();
    }

    this->fullscreenEnabled = fullscreenEnabled;
    this->classAtom = classAtom;

    int fullscreenWidth = GetSystemMetrics(SM_CXSCREEN);
    int fullscreenHeight = GetSystemMetrics(SM_CYSCREEN);

    int x = 0;
    int y = 0;

    if (this->fullscreenEnabled) {
        this->width = fullscreenWidth;
        this->height = fullscreenHeight;

        DEVMODE screenSettings = {};
        screenSettings.dmSize = sizeof(DEVMODE);
        screenSettings.dmBitsPerPel = 32;
        screenSettings.dmPelsWidth = (unsigned long)this->width;
        screenSettings.dmPelsHeight = (unsigned long)this->height;
        screenSettings.dmFields = DM_BITSPERPEL | DM_PELSWIDTH | DM_PELSHEIGHT;
        ChangeDisplaySettings(&screenSettings, CDS_FULLSCREEN);
    }
    else {
        this->width = width;
        this->height = height;

        x = (fullscreenWidth - this->width) / 2;
        y = (fullscreenHeight - this->height) / 2;
    }

    handle = CreateWindowEx(WS_EX_APPWINDOW, MAKEINTATOM(this->classAtom), applicationName, WS_CLIPSIBLINGS | WS_CLIPCHILDREN | WS_POPUP, x, y, this->width, this->height, NULL, NULL, applicationInstanceHandle, NULL);
    ShowWindow(handle, showCmd);
    SetForegroundWindow(handle);
    SetFocus(handle);
    ShowCursor(true);

    //RAWINPUTDEVICE rid = {};
    //rid.usUsagePage = 0x01;  // Generic Desktop Controls
    //rid.usUsage = 0x02;      // Mouse
    //rid.dwFlags = RIDEV_INPUTSINK;
    //rid.hwndTarget = handle;  // Передаём окно, которое будет получать сообщения

    //if (!RegisterRawInputDevices(&rid, 1, sizeof(rid))) {
    //    OutputDebugString(L"Failed to register raw input!");
    //}

    setInitialized();
}

void Window::release() {
    if (isReleased()) {
        return;
    }

    ShowCursor(true);

    if (fullscreenEnabled) {
        ChangeDisplaySettings(NULL, 0);
    }

    DestroyWindow(handle);
    handle = NULL;

    fullscreenEnabled = false;

    width = 0;
    height = 0;
    handle = NULL;

    classAtom = 0;

    setReleased();
}
