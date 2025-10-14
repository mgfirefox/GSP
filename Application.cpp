#include "Application.h"

const bool Application::fullscreenEnabled = false;

const bool Application::vsyncEnabled = true;

const uint32 Application::fpsLimit = 400;

const int32 Application::windowWidth = 1440;
const int32 Application::windowHeight = 900;

const std::string Application::closeApplicationActionName = "closeApplication";

std::shared_ptr<Application> Application::application;

Application::Application(std::shared_ptr<Input> input, std::shared_ptr<Timer> timer,
                         std::shared_ptr<FpsCounter> fpsCounter) : name(), window(), renderer()
{
    initialized = false;
    released = false;

    this->input = input;
    this->timer = timer;
    this->fpsCounter = fpsCounter;

    instanceHandle = nullptr;

    comInitializationResult = E_UNEXPECTED;

    shouldShutdown = false;
}

Application::~Application()
{
    release();

    fpsCounter.reset();
    timer.reset();
    input.reset();
}

bool Application::isInitialized()
{
    return initialized;
}

void Application::setInitialized()
{
    initialized = true;
    released = false;
}

bool Application::isReleased()
{
    return released;
}

void Application::setReleased()
{
    initialized = false;
    released = true;
}

bool Application::initialize(HINSTANCE instanceHandle, std::wstring name,
                             std::shared_ptr<Application> application, int32
                             showCmd)
{
    if (isInitialized())
    {
        release();
    }

    this->instanceHandle = instanceHandle;
    this->name = std::move(name);

    bool result = initializeWindow(showCmd);
    if (!result)
    {
        return false;
    }

    result = initializeRenderer();
    if (!result)
    {
        return false;
    }

    result = bindActions();
    if (!result)
    {
        return false;
    }

    this->application = application;

    result = initializeCom();
    if (!result)
    {
        return false;
    }

    setInitialized();
    return true;
}

void Application::run()
{
    while (!shouldShutdown)
    {
        bool result = sendWindowsMessages();
        if (!result)
        {
            break;
        }

        timer->update();

        input->onInputEvents();
        if (shouldShutdown)
        {
            break;
        }

        result = renderer->renderFrame();
        if (!result)
        {
            break;
        }
        if (shouldShutdown)
        {
            break;
        }

        /*wchar_t c[32];
        swprintf_s(c, 32, L"%d", fpsCounter->getFps());
        c[31] = '\0';
        OutputDebugString(L"\n\nFPS: ");
        OutputDebugString(c);*/

        /*if (!vsyncEnabled && fpsLimit != 0) {
            fpsCounter->limit(fpsLimit);
        }*/

        fpsCounter->update();
    }
}

void Application::release()
{
    if (isReleased())
    {
        return;
    }

    shouldShutdown = false;

    if (SUCCEEDED(comInitializationResult))
    {
        CoUninitialize();
    }
    comInitializationResult = E_UNEXPECTED;

    renderer.reset();
    window.reset();

    name.clear();
    name.shrink_to_fit();
    instanceHandle = nullptr;

    application.reset();

    setReleased();
}

bool Application::initializeWindow(int32 showCmd)
{
    WNDCLASSEX windowClassDesc = {};
    windowClassDesc.cbSize = sizeof(WNDCLASSEX);
    windowClassDesc.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
    windowClassDesc.lpfnWndProc = windowProcedure;
    windowClassDesc.cbClsExtra = 0;
    windowClassDesc.cbWndExtra = 0;
    windowClassDesc.hInstance = this->instanceHandle;
    windowClassDesc.hIcon = LoadIcon(nullptr, IDI_WINLOGO);
    windowClassDesc.hIconSm = windowClassDesc.hIcon;
    windowClassDesc.hCursor = LoadCursor(nullptr, IDC_ARROW);
    windowClassDesc.hbrBackground = static_cast<HBRUSH>(GetStockObject(BLACK_BRUSH));
    windowClassDesc.lpszMenuName = nullptr;
    windowClassDesc.lpszClassName = this->name.c_str();

    window = createSharedPointer<Window>();
    if (fullscreenEnabled)
    {
        return window->initializeFullscreen(windowClassDesc, instanceHandle, name, showCmd,
                                            windowWidth, windowHeight);
    }

    return window->initializeWindowed(windowClassDesc, instanceHandle, name, showCmd, windowWidth,
                                      windowHeight);
}

bool Application::initializeRenderer()
{
    renderer = createSharedPointer<Renderer>(window, input, timer);
    bool result = renderer->initialize(vsyncEnabled);
    if (!result)
    {
        return false;
    }

    return true;
}

bool Application::bindActions()
{
    ActionFunction closeApplicationFunction = [=](std::string actionName, InputObject inputObject)
    {
        if (inputObject.inputState == InputState::End)
        {
            shouldShutdown = true;
        }
    };

    std::array<int, 1> keyCodes = {VK_ESCAPE};

    input->bindAction(closeApplicationActionName, closeApplicationFunction, keyCodes.data(),
                      keyCodes.size());

    return true;
}

bool Application::initializeCom()
{
    comInitializationResult = CoInitializeEx(nullptr, COINIT_MULTITHREADED);
    if (FAILED(comInitializationResult))
    {
        return false;
    }

    return true;
}

bool Application::sendWindowsMessages()
{
    do
    {
        MSG windowMessage = {};

        int32 result = PeekMessage(&windowMessage, window->getHandle(), 0, 0, PM_REMOVE);
        if (result == 0)
        {
            return true;
        }
        if (result == -1)
        {
            OutputDebugString(L"Proper error handling needed");

            continue;
        }

        TranslateMessage(&windowMessage);
        DispatchMessage(&windowMessage);

        if (windowMessage.message == WM_QUIT)
        {
            shouldShutdown = true;
        }
    }
    while (!shouldShutdown);

    return false;
}

LRESULT Application::windowProcedure(HWND windowHandle, UINT message, WPARAM wparam, LPARAM lparam)
{
    return application->onAllWindowMessages(windowHandle, message, wparam, lparam);
}

LRESULT Application::onAllWindowMessages(HWND windowHandle, UINT message, WPARAM wparam,
                                         LPARAM lparam)
{
    switch (message)
    {
    // Window Notifications
    case WM_CLOSE:
    case WM_DESTROY:
        {
            PostQuitMessage(0);

            break;
        }

    // Keyboard Input Notifications
    case WM_KEYDOWN:
        {
            WORD keyFlags = HIWORD(lparam);
            if (keyFlags & KF_REPEAT)
            {
                //OutputDebugString(L"\nGotcha");

                break;
            }
        }
    case WM_KEYUP:

    // Mouse Input Notifications
    case WM_LBUTTONDOWN:
    case WM_LBUTTONUP:
    case WM_MBUTTONDOWN:
    case WM_MBUTTONUP:
    case WM_MOUSEMOVE:
    case WM_MOUSEWHEEL:
    case WM_RBUTTONDOWN:
    case WM_RBUTTONUP:
        {
            InputEvent inputEvent = {};
            inputEvent.message = message;
            inputEvent.wparam = wparam;
            inputEvent.lparam = lparam;

            input->addInputEvent(inputEvent);

            break;
        }

    // Raw Input Notifications
    /*case WM_INPUT:
        input->onRawInputNotifications(message, wparam, lparam);
        break;*/

    default: return DefWindowProc(windowHandle, message, wparam, lparam);
    }

    return 0;
}
